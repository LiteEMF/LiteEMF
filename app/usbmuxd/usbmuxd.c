/*
 * device.c
 *
 * Copyright (C) 2009 Hector Martin "marcan" <hector@marcansoft.com>
 * Copyright (C) 2014 Mikkel Kamstrup Erlandsen <mikkel.kamstrup@xamarin.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 or version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

// @ref: https://github.dev/libimobiledevice/usbmuxd

#define _DEFAULT_SOURCE
#define _BSD_SOURCE


#include "hw_config.h"
#if USBH_TYPE_SUPPORT & BIT_ENUM(DEV_TYPE_USBMUXD)
#include "api/usb/host/usbh.h"

#include "api/api_tick.h"
#include "tcp.h"
#include "usbmuxd.h"

#include "api/api_log.h"


/*******************************************************************************************************************
**	Hardware  Defined
********************************************************************************************************************/
enum loglevel {
	LL_FATAL = 0,
	LL_ERROR,
	LL_WARNING,
	LL_NOTICE,
	LL_INFO,
	LL_DEBUG,
	LL_SPEW,
	LL_FLOOD,
};
#define usbmuxd_log(level,str,args...)		logd(str "\n",args)

#define 	htonl	SWAP32_L		//ok
#define 	htons	SWAP16_L
#define 	ntohl	SWAP32_L
#define 	ntohs	SWAP16_L


#define POLLIN		0x0001	// 数据可读
#define POLLPRI		0X0002  //紧急数据可读 */
#define POLLOUT		0x0004	//写操作不会阻塞 */
#define POLLERR		0x0008	//错误条件发生（只对输出） */
#define POLLHUP		0x0010	//挂起（只对输出） */
#define POLLNVAL	0x0020	//无效请求：fd没有打开（只对输出） */



#define ACK_TIMEOUT 30
#define CONN_ACK_PENDING 1

/*******************************************************************************************************************
**	Parameters def
********************************************************************************************************************/

enum mux_phone_type {
	PHONE_NONE=0,
	PHONE_640x1136PX,			// iphone5/5s
	PHONE_750x1334PX,			// iphone6/6s/7/8
	PHONE_1080x1920PX,			// iphone6/6+/7+/8+
	PHONE_1125x2436PX,			// iphoneX/Xs
	PHONE_828x1792PX,			// iphoneXr
	PHONE_1242x2688PX,			// iphoneXsMax
	
	PHONE_1024x768PX,			// iPad 1/2/mini/
	PHONE_2048x1536PX,			// iPad 4/Air/mini2/mini3/mini4/Air2/
	PHONE_2732x2048PX,			// iPad Pro
};


enum mux_protocol {
	MUX_PROTO_VERSION = 0,
	MUX_PROTO_CONTROL = 1,
	MUX_PROTO_SETUP = 2,
	MUX_PROTO_TCP = IPPROTO_TCP,
};

enum mux_dev_state {
	MUXDEV_INIT,	// sent version packet
	MUXDEV_ACTIVE,	// received version packet, active
	MUXDEV_DEAD		// dead
};

enum mux_conn_state {
	CONN_CONNECTING,	// SYN
	CONN_CONNECTED,		// SYN/SYNACK/ACK -> active
	CONN_REFUSED,		// RST received during SYN
	CONN_DYING,			// RST received
	CONN_DEAD			// being freed; used to prevent infinite recursion between client<->device freeing
};

struct mux_header
{
	uint32_t protocol;
	uint32_t length;
	uint32_t magic;
	uint16_t tx_seq;
	uint16_t rx_seq;
};

struct version_header
{
	uint32_t major;
	uint32_t minor;
	uint32_t padding;
};


struct mux_connection
{
	struct mux_device *dev;
	struct mux_client *client;
	enum mux_conn_state state;
	uint16_t sport, dport;
	uint32_t tx_seq, tx_ack, tx_acked, tx_win;
	uint32_t rx_seq, rx_recvd, rx_ack, rx_win;
	uint32_t max_payload;
	uint32_t sendable;
	int flags;
	short events;
	uint64_t last_ack_time;
};

struct collection {
	void **list;
	int capacity;
};

struct mux_device
{
	uint8_t *usbdev;
	int id;
	enum mux_dev_state state;
	int visible;
	struct collection connections;
	unsigned char *pktbuf;
	uint32_t pktlen;
	int version;
	uint16_t rx_seq;
	uint16_t tx_seq;
};

/*******************************************************************************************************************
**	Parameters
********************************************************************************************************************/
bool usbmuxd_config_ready;
enum mux_phone_type	mux_phone;

struct mux_device mux_dev;
struct mux_connection mux_conn;
uint8_t mux_pak_buf[DEV_MRU];


/*****************************************************************************************************
**  Function
******************************************************************************************************/
extern void device_data_input(uint8_t *buffer, uint32_t length);
error_t usbmuxd_write(const void *buf, uint32_t length)
{
    return usbh_class_out(DEV_TYPE_USBMUXD, 0, (uint8_t*)buf, length);
}

void usbmuxd_data_process(uint8_t* buf,uint16_t len)
{
	device_data_input(buf, (uint32_t)len);
}


enum mux_phone_type get_phone_type(char *name)
{
	enum mux_phone_type type = PHONE_1125x2436PX;
	if(0 == memcmp(name, "iPhone9,2",strlen(name))){			//iphone7p
		type = PHONE_1080x1920PX;
	}else if(0 == memcmp(name, "iPhone10,3",strlen(name))){		//iphoneX
		type = PHONE_1125x2436PX;
	}else if(0 == memcmp(name, "iPhone9,1",strlen(name))){		//iphone7
		type = PHONE_750x1334PX;
	}else if(0 == memcmp(name, "iPad4,4",strlen(name))){		//iPad mini
		type = PHONE_1024x768PX;
	}
	return type;
}




static int send_packet(struct mux_device *dev, enum mux_protocol proto, void *header, const void *data, int length)
{
	unsigned char buffer[USB_MTU];
	int hdrlen;
	int res;

	switch(proto) {
		case MUX_PROTO_VERSION:
			hdrlen = sizeof(struct version_header);
			break;
		case MUX_PROTO_SETUP:
			hdrlen = 0;
			break;
		case MUX_PROTO_TCP:
			hdrlen = sizeof(struct tcphdr);
			break;
		default:
			usbmuxd_log(LL_ERROR, "Invalid protocol %d for outgoing packet (dev %d hdr %p data %p len %d)", proto, dev->id, header, data, length);
			return -1;
	}
	int mux_header_size = ((dev->version < 2) ? 8 : sizeof(struct mux_header));

	int total = mux_header_size + hdrlen + length;

	if(total > USB_MTU) {
		usbmuxd_log(LL_ERROR, "Tried to send packet larger than USB MTU (hdr %d data %d total %d) to device %d", hdrlen, length, total, dev->id);
		return -1;
	}

	struct mux_header *mhdr = (struct mux_header *)buffer;
	mhdr->protocol = htonl(proto);
	mhdr->length = htonl(total);
	if (dev->version >= 2) {
		mhdr->magic = htonl(0xfeedface);
		if (proto == MUX_PROTO_SETUP) {
			dev->tx_seq = 0;
			dev->rx_seq = 0xFFFF;
		}
		mhdr->tx_seq = htons(dev->tx_seq);
		mhdr->rx_seq = htons(dev->rx_seq);
		dev->tx_seq++;
	}	
	memcpy(buffer + mux_header_size, header, hdrlen);
	if(data && length)
		memcpy(buffer + mux_header_size + hdrlen, data, length);
	if((res = usbmuxd_write(buffer, total)) != 0) {
		usbmuxd_log(LL_ERROR, "usb_send failed while sending packet (len %d) to device %d: %d", total, dev->id, res);
		return res;
	}
	return total;
}

int send_anon_rst(struct mux_device *dev, uint16_t sport, uint16_t dport, uint32_t ack)
{
	struct tcphdr th;
	memset(&th, 0, sizeof(th));
	th.th_sport = htons(sport);
	th.th_dport = htons(dport);
	th.th_ack = htonl(ack);
	th.th_flags = TH_RST;
	th.th_off = sizeof(th) / 4;

	//usbmuxd_log(LL_DEBUG, "[OUT] dev=%d sport=%d dport=%d flags=0x%x", dev->id, sport, dport, th.th_flags);

	int res = send_packet(dev, MUX_PROTO_TCP, &th, NULL, 0);
	return res;
}

static int send_tcp(struct mux_connection *conn, uint8_t flags, const unsigned char *data, int length)
{
	struct tcphdr th;
	memset(&th, 0, sizeof(th));
	th.th_sport = htons(conn->sport);
	th.th_dport = htons(conn->dport);
	th.th_seq = htonl(conn->tx_seq);
	th.th_ack = htonl(conn->tx_ack);
	th.th_flags = flags;
	th.th_off = sizeof(th) / 4;
	th.th_win = htons(conn->tx_win >> 8);

	//usbmuxd_log(LL_DEBUG, "[OUT] dev=%d sport=%d dport=%d seq=%d ack=%d flags=0x%x window=%d[%d] len=%d",
	//	conn->dev->id, conn->sport, conn->dport, conn->tx_seq, conn->tx_ack, flags, conn->tx_win, conn->tx_win >> 8, length);

	int res = send_packet(conn->dev, MUX_PROTO_TCP, &th, data, length);
	if(res >= 0) {
		conn->tx_acked = conn->tx_ack;
		conn->last_ack_time = m_systick;
		conn->flags &= ~CONN_ACK_PENDING;
	}
	return res;
}

static void connection_teardown(struct mux_connection *conn)
{
	int res;
	if(conn->state == CONN_DEAD)
		return;
	usbmuxd_log(LL_DEBUG, "connection_teardown dev %d sport %d dport %d", conn->dev->id, conn->sport, conn->dport);
	if(conn->dev->state != MUXDEV_DEAD && conn->state != CONN_DYING && conn->state != CONN_REFUSED) {
		res = send_tcp(conn, TH_RST, NULL, 0);
		if(res < 0)
			usbmuxd_log(LL_ERROR, "Error sending TCP RST to device %d (%d->%d)", conn->dev->id, conn->sport, conn->dport);
	}
	conn->state = CONN_REFUSED;		// RST received during SYN
}

int device_start_connect(int device_id, uint16_t dport, struct mux_client *client)
{
	struct mux_device *dev = &mux_dev;
	uint16_t sport = TCP_SPORT;			
	struct mux_connection *conn;
	conn = &mux_conn;
	memset(conn, 0, sizeof(struct mux_connection));

	conn->dev = dev;
	conn->client = client;
	conn->state = CONN_CONNECTING;
	conn->sport = sport;
	conn->dport = dport;
	conn->tx_seq = 0;
	conn->tx_ack = 0;
	conn->tx_acked = 0;
	conn->tx_win = 131072;
	conn->rx_recvd = 0;
	conn->flags = 0;
	conn->max_payload = USB_MTU - sizeof(struct mux_header) - sizeof(struct tcphdr);

	int res;

	res = send_tcp(conn, TH_SYN, NULL, 0);
	if(res < 0) {
		usbmuxd_log(LL_ERROR, "Error sending TCP SYN to device %d (%d->%d)", dev->id, sport, dport);
		return -RESULT_CONNREFUSED; //bleh
	}
	return 0;
}

/**
 * Examine the state of a connection's buffers and
 * update all connection flags and masks accordingly.
 * Does not do I/O.
 *
 * @param conn The connection to update.
 */
static void update_connection(struct mux_connection *conn)
{
	if(conn->tx_acked != conn->tx_ack)
		conn->flags |= CONN_ACK_PENDING;
	else
		conn->flags &= ~CONN_ACK_PENDING;
}

static int send_tcp_ack(struct mux_connection *conn)
{
	if(send_tcp(conn, TH_ACK, NULL, 0) < 0) {
		usbmuxd_log(LL_ERROR, "Error sending TCP ACK (%d->%d)", conn->sport, conn->dport);
		connection_teardown(conn);
		return -1;
	}

	update_connection(conn);

	return 0;
}





/**
 * Copy a payload to a connection's in-buffer and
 * set the POLLOUT event mask on the connection so
 * the next main_loop iteration will dispatch the
 * buffer if the connection socket is writable.
 *
 * Connection buffers are flushed in the
 * device_client_process() function.
 *
 * @param conn The connection to add incoming data to.
 * @param payload Payload to prepare for writing.
 *   The payload will be copied immediately so you are
 *   free to alter or free the payload buffer when this
 *   function returns.
 * @param payload_length number of bytes to copy from from
 *   the payload.
 */
static void connection_device_input(struct mux_connection *conn, unsigned char *payload, uint32_t payload_length)
{
	if(TCP_SYS_DPORT == conn->dport){
		char *p,*typep,*type_endp;
		uint8_t type_len;
		char mux_phone_type[20];

		if(payload_length < 4) return;
		
		payload[payload_length-1] = 0;	//string end
		
		//if((payload[0] > 0x08) && (payload[0] < 0x7f)) logd(":%s\n",payload);

		p = strstr((const char*)payload+4,"Value</key>");
		if(NULL != p){
			typep = strstr(p,"<string>");
			if(NULL != typep){
				type_endp = strstr(typep, "</string>");
				if(NULL != type_endp){
					type_len = type_endp - typep - strlen("<string>");
					type_len = MIN(type_len,20);
					memset(mux_phone_type,0,sizeof(mux_phone_type));
					memcpy(mux_phone_type, typep+strlen("<string>"),type_len);
					
					mux_phone = get_phone_type(mux_phone_type);
					logd("iphone type: %s ...\n",mux_phone_type);
					logd("phone type = %d\n",mux_phone);
				}
			}
		}
		send_tcp_ack(conn);
		if(PHONE_NONE != mux_phone){	//获取成功
			connection_teardown(conn);
			// TODO 获取设备信息后退出
		}
	}else if(TCP_APP_DPORT == conn->dport){
		logd("muxd payload:",mux_phone);dumpd(payload,payload_length);
		// TODO do something for user decode
		// Device likes it best when we are prompty ACKing data
		send_tcp_ack(conn);
	}
	//update_connection(conn);
}

void device_abort_connect(int device_id, struct mux_client *client)
{
	struct mux_connection *conn = NULL;	//TODO ???	// = get_mux_connection(device_id, client);
	if (NULL != conn) {
		connection_teardown(conn);
	} else {
		usbmuxd_log(LL_WARNING, "Attempted to abort for nonexistent connection for device %d", device_id);
	}
}

static void device_version_input(struct mux_device *dev, struct version_header *vh)
{
	if(dev->state != MUXDEV_INIT) {
		usbmuxd_log(LL_WARNING, "Version packet from already initialized device %d", dev->id);
		return;
	}
	vh->major = ntohl(vh->major);
	vh->minor = ntohl(vh->minor);
	if(vh->major != 2 && vh->major != 1) {
		usbmuxd_log(LL_ERROR, "Device %d has unknown version %d.%d", dev->id, vh->major, vh->minor);
		return;
	}
	dev->version = vh->major;

	if (dev->version >= 2) {
		send_packet(dev, MUX_PROTO_SETUP, NULL, "\x07", 1);
	}

	usbmuxd_log(LL_NOTICE, "Connected to v%d.%d device %d", dev->version, vh->minor, dev->id);

	dev->state = MUXDEV_ACTIVE;
#if 0	
	collection_init(&dev->connections);
	struct device_info info;
	info.id = dev->id;
	info.location = usb_get_location(dev->usbdev);
	info.serial = usb_get_serial(dev->usbdev);
	info.pid = usb_get_pid(dev->usbdev);
	info.speed = usb_get_speed(dev->usbdev);
	preflight_worker_device_add(&info);
#endif
}

/**
 * Handle an incoming TCP packet from the device.
 *
 * @param dev The device handle TCP input on.
 * @param th Pointer to the TCP header struct.
 * @param payload Payload data.
 * @param payload_length Number of bytes in payload.
 */
static void device_tcp_input(struct mux_device *dev, struct tcphdr *th, unsigned char *payload, uint32_t payload_length)
{
	uint16_t sport = ntohs(th->th_dport);
	uint16_t dport = ntohs(th->th_sport);
	struct mux_connection* conn = &mux_conn;

	//usbmuxd_log(LL_DEBUG, "[IN] dev=%d sport=%d dport=%d seq=%d ack=%d flags=0x%x window=%d[%d] len=%d",
	//	dev->id, dport, sport, ntohl(th->th_seq), ntohl(th->th_ack), th->th_flags, ntohs(th->th_win) << 8, ntohs(th->th_win), payload_length);

	if(dev->state != MUXDEV_ACTIVE) {
		usbmuxd_log(LL_ERROR, "Received TCP packet from device %d but the device isn't active yet, discarding", dev->id);
		return;
	}

	conn->rx_seq = ntohl(th->th_seq);
	conn->rx_ack = ntohl(th->th_ack);
	conn->rx_win = ntohs(th->th_win) << 8;

	if(th->th_flags & TH_RST) {
		payload[payload_length-1] = 0;
		usbmuxd_log(LL_DEBUG, "RST reason: %s", payload+1);
	}

	if(conn->state == CONN_CONNECTING) {
		if(th->th_flags != (TH_SYN|TH_ACK)) {
			if(th->th_flags & TH_RST)
				conn->state = CONN_REFUSED;
			//usbmuxd_log(LL_INFO, "Connection refused by device %d (%d->%d)", dev->id, sport, dport);
			connection_teardown(conn); //this also sends the notification to the client
		} else {
			conn->tx_seq++;
			conn->tx_ack++;
			conn->rx_recvd = conn->rx_seq;
			if(send_tcp(conn, TH_ACK, NULL, 0) < 0) {
				usbmuxd_log(LL_ERROR, "Error sending TCP ACK to device %d (%d->%d)", dev->id, sport, dport);
				connection_teardown(conn);
				return;
			}
			conn->state = CONN_CONNECTED;
			
			usbmuxd_log(LL_SPEW, "client_notify_connect %X ok...",conn->dport);
			update_connection(conn);
		}
	} else if(conn->state == CONN_CONNECTED) {
		if(th->th_flags != TH_ACK) {
			//usbmuxd_log(LL_INFO, "Connection reset by device %d (%d->%d)", dev->id, sport, dport);
			if(th->th_flags & TH_RST)
				conn->state = CONN_DYING;
			connection_teardown(conn);
		} else {
			connection_device_input(conn, payload, payload_length);
		}
	}
}

/**
 * Take input data from the device that has been read into a buffer
 * and dispatch it to the right protocol backend (eg. TCP).
 *
 * @param usbdev
 * @param buffer
 * @param length
 */
void device_data_input(uint8_t *buffer, uint32_t length)
{
	struct mux_device *dev = &mux_dev;
	
	if(!length)
		return;

	// handle broken up transfers
	if(dev->pktlen) {
		if((length + dev->pktlen) > DEV_MRU) {
			usbmuxd_log(LL_ERROR, "Incoming split packet is too large (%d so far), dropping!", length + dev->pktlen);
		}else{			//err continue parse data in
			memcpy(dev->pktbuf + dev->pktlen, buffer, length);
		}
		
		struct mux_header *mhdr = (struct mux_header *)dev->pktbuf;
		if((length < USB_MRU) || (ntohl(mhdr->length) == (length + dev->pktlen))) {
			buffer = dev->pktbuf;
			length += dev->pktlen;
			dev->pktlen = 0;
			//usbmuxd_log(LL_SPEW, "Gathered mux data from buffer (total size: %d)", length);
		} else {
			dev->pktlen += length;
			//usbmuxd_log(LL_SPEW, "Appended mux data to buffer (total size: %d)", dev->pktlen);
			return;
		}
	} else {
		struct mux_header *mhdr = (struct mux_header *)buffer;
		if((length == USB_MRU) && (length < ntohl(mhdr->length))) {
			memcpy(dev->pktbuf, buffer, length);
			dev->pktlen = length;
			//usbmuxd_log(LL_SPEW, "Copied mux data to buffer (size: %d)", dev->pktlen);
			return;
		}
	}

	struct mux_header *mhdr = (struct mux_header *)buffer;
	int mux_header_size = ((dev->version < 2) ? 8 : sizeof(struct mux_header));
	if(ntohl(mhdr->length) != length) {
		usbmuxd_log(LL_ERROR, "Incoming packet size mismatch (dev %d, expected %d, got %d)", dev->id, ntohl(mhdr->length), length);
		return;
	}

	struct tcphdr *th;
	unsigned char *payload;
	uint32_t payload_length;

	if (dev->version >= 2) {
		dev->rx_seq = ntohs(mhdr->rx_seq);
	}

	switch(ntohl(mhdr->protocol)) {
		case MUX_PROTO_VERSION:
			if(length < (mux_header_size + sizeof(struct version_header))) {
				usbmuxd_log(LL_ERROR, "Incoming version packet is too small (%d)", length);
				return;
			}
			device_version_input(dev, (struct version_header *)((char*)mhdr+mux_header_size));
			break;
		case MUX_PROTO_CONTROL:
			payload = (unsigned char *)(mhdr+1);
			payload_length = length - mux_header_size;
			break;
		case MUX_PROTO_TCP:
			if(length < (mux_header_size + sizeof(struct tcphdr))) {
				usbmuxd_log(LL_ERROR, "Incoming TCP packet is too small (%d)", length);
				return;
			}
			th = (struct tcphdr *)((char*)mhdr+mux_header_size);
			payload = (unsigned char *)(th+1);
			length = MIN(length,DEV_MRU);			//for len overflow parse
			payload_length = length - sizeof(struct tcphdr) - mux_header_size;
			device_tcp_input(dev, th, payload, payload_length);
			break;
		default:
			usbmuxd_log(LL_ERROR, "Incoming packet for device %d has unknown protocol 0x%x)", dev->id, ntohl(mhdr->protocol));
			break;
	}

}

int device_add(uint8_t *usbdev)
{
	int res;
	int id = DEVICE_ID;		//get_next_device_id();
	struct mux_device *dev;
	
	dev = &mux_dev;
	dev->id = id;
	dev->usbdev = usbdev;
	dev->state = MUXDEV_INIT;
	dev->visible = 0;
	dev->pktbuf = mux_pak_buf;
	dev->pktlen = 0;
	dev->version = 0;
	struct version_header vh;
	vh.major = htonl(1);		// fix 2 to 1
	vh.minor = htonl(0);
	vh.padding = 0;
	if((res = send_packet(dev, MUX_PROTO_VERSION, &vh, NULL, 0)) < 0) {
		usbmuxd_log(LL_ERROR, "Error sending version request packet to device %d", id);
		return res;
	}
	return 0;
}





/*--------------------------------------------------------------------------------------------------*/


const_t uint8_t plist_file[]={
"\x00\x00\x01\x49<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n\
<plist version=\"1.0\">\n\
<dict>\n\
	<key>Label</key>\n\
	<string>ideviceinfo</string>\n\
	<key>Key</key>\n\
	<string>ProductType</string>\n\
	<key>Request</key>\n\
	<string>GetValue</string>\n\
</dict>\n\
</plist>\n\
"};






void usbmuxd_init(void)
{
	mux_dev.state = MUXDEV_DEAD;
	mux_conn.state = CONN_REFUSED;
	mux_phone=PHONE_NONE;
	usbmuxd_config_ready = false;
}



bool usbmuxd_send_data(uint8_t index,uint8_t* buf,uint16_t len)
{
	bool ret = false;
	if((CONN_CONNECTED == mux_conn.state) && (TCP_APP_DPORT == mux_conn.dport)){
		if(0 == send_tcp(&mux_conn, TH_ACK, buf,len)){
			ret = true;
		}
	}
	return ret;
}




void usbmuxd_handler(uint32_t period_10us)
{
	static timer_t muxd_t;
	uint8_t err = ERROR_NOT_FOUND;
	
	if((m_systick - muxd_t) >= period_10us/100){
		muxd_t = m_systick;

		if(MUXDEV_ACTIVE != mux_dev.state){
			uint8_t id;
			usbh_class_t *pcalss = NULL;
			id = usbh_class_find_by_type_all(DEV_TYPE_AOA,0,&pcalss);
			if(NULL != pcalss){
				mux_conn.state = CONN_REFUSED;
				device_add(&id);
			}
			return;
		}

		if(PHONE_NONE == mux_phone){
			if((MUXDEV_ACTIVE == mux_dev.state) && (CONN_REFUSED == mux_conn.state)){
				logd("device_start_connect...%X\n",TCP_SYS_DPORT);
				device_start_connect(DEVICE_ID, TCP_SYS_DPORT, NULL);		// add
			}else if(CONN_CONNECTED == mux_conn.state){
				send_tcp(&mux_conn, TH_ACK, (uint8_t*)plist_file,sizeof(plist_file)-1);
			}
		}else{
			if((MUXDEV_ACTIVE == mux_dev.state) && (CONN_REFUSED == mux_conn.state)){
				logd("device_start_connect...%X\n",TCP_APP_DPORT);
				device_start_connect(DEVICE_ID, TCP_APP_DPORT, NULL);		// add
			}else if(CONN_CONNECTED == mux_conn.state){
				
			}
		}
	}
}

#endif
