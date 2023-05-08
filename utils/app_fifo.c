/* Copyright (c) 2013 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
 

#include "emf_typedef.h"
#include "utils/app_fifo.h"

/**@brief Put one byte to the FIFO. */
static __INLINE void fifo_put(app_fifo_t * p_fifo, uint8_t byte)
{
    p_fifo->p_buf[p_fifo->write_pos++] = byte;
    if(p_fifo->write_pos >= p_fifo->buf_size_max){
    	p_fifo->write_pos = 0;
    }
    
	if (p_fifo->write_pos == p_fifo->read_pos){
	   p_fifo->fifo_stu = FIFO_FULL;
    }else{
    	p_fifo->fifo_stu = FIFO_AVABLE;
    }
}


/**@brief Look at one byte in the FIFO. */
static __INLINE void fifo_peek(app_fifo_t * p_fifo, uint16_t index, uint8_t * p_byte)
{
    *p_byte = p_fifo->p_buf[(p_fifo->read_pos + index)];
}


/**@brief Get one byte from the FIFO. */
static __INLINE void fifo_get(app_fifo_t * p_fifo, uint8_t * p_byte)
{
    fifo_peek(p_fifo, 0, p_byte);
    p_fifo->read_pos++;
	if(p_fifo->read_pos >= p_fifo->buf_size_max){
	   p_fifo->read_pos = 0;
	}

	if (p_fifo->write_pos == p_fifo->read_pos){
	  p_fifo->fifo_stu = FIFO_EMPTY;
	}else{
	   p_fifo->fifo_stu = FIFO_AVABLE;
	}
}


uint8_t app_fifo_init(app_fifo_t * p_fifo, uint8_t * p_buf, uint16_t buf_size)
{
	if(p_fifo == NULL) return ERROR_NULL;
    if(p_buf == NULL) return ERROR_NULL;

    if(buf_size){
        p_fifo->p_buf         = p_buf;
    }else{
        p_fifo->p_buf         = NULL;
    }
    p_fifo->buf_size_max = buf_size;
    p_fifo->read_pos      = 0;
    p_fifo->write_pos     = 0;
  	p_fifo->fifo_stu	  =	FIFO_EMPTY;

    return ERROR_SUCCESS;
}


uint8_t app_fifo_put(app_fifo_t * p_fifo, uint8_t byte)
{
	if(p_fifo->p_buf == NULL) return ERROR_NULL;
    if (FIFO_FULL != p_fifo->fifo_stu)
    {
        fifo_put(p_fifo, byte);
        return ERROR_SUCCESS;
    }

    return ERROR_NO_MEM;
}


uint8_t app_fifo_get(app_fifo_t * p_fifo, uint8_t * p_byte)
{
    if(p_fifo->p_buf == NULL) return ERROR_NULL;
    if (FIFO_EMPTY != p_fifo->fifo_stu)
    {
        fifo_get(p_fifo, p_byte);
        return ERROR_SUCCESS;
    }

    return ERROR_NOT_FOUND;

}


uint8_t app_fifo_peek(app_fifo_t * p_fifo, uint16_t index, uint8_t * p_byte)
{
	if(p_fifo->p_buf == NULL) return ERROR_NULL;
    if (fifo_length(p_fifo) > index)
    {
        fifo_peek(p_fifo, index, p_byte);
        return ERROR_SUCCESS;
    }

    return ERROR_NOT_FOUND;
}


uint8_t app_fifo_flush(app_fifo_t * p_fifo)
{
    p_fifo->read_pos = p_fifo->write_pos;
    p_fifo->fifo_stu = FIFO_EMPTY;
    return ERROR_SUCCESS;
}


uint8_t app_fifo_read(app_fifo_t * p_fifo, uint8_t * p_byte_array, uint16_t * p_size)
{
    const uint16_t byte_count    = fifo_length(p_fifo);
    const uint16_t requested_len = (*p_size);
    uint16_t       index         = 0;
    uint16_t       read_size     = MIN(requested_len, byte_count);
	
	//VERIFY_PARAM_NOT_NULL(p_fifo);
    //VERIFY_PARAM_NOT_NULL(p_size);
	
	if(p_fifo->p_buf == NULL) return ERROR_NULL;
	
    (*p_size) = byte_count;

    // Check if the FIFO is empty.
    if (byte_count == 0)
    {
        return ERROR_NOT_FOUND;
    }

    // Check if application has requested only the size.
    if (p_byte_array == NULL)
    {
        return ERROR_SUCCESS;
    }

    // Fetch bytes from the FIFO.
    while (index < read_size)
    {
        fifo_get(p_fifo, &p_byte_array[index++]);
    }

    (*p_size) = read_size;

    return ERROR_SUCCESS;
}


uint8_t app_fifo_write(app_fifo_t * p_fifo, uint8_t const * p_byte_array, uint16_t * p_size)
{
    const uint16_t available_count = p_fifo->buf_size_max - fifo_length(p_fifo);
    const uint16_t requested_len   = (*p_size);
    uint16_t       index           = 0;
    uint16_t       write_size      = MIN(requested_len, available_count);
	
	//VERIFY_PARAM_NOT_NULL(p_fifo);
    //VERIFY_PARAM_NOT_NULL(p_size);

	if(p_fifo->p_buf == NULL) return ERROR_NULL;
	
    (*p_size) = available_count;

    // Check if the FIFO is FULL.
    if (available_count == 0)
    {
        return ERROR_NO_MEM;
    }

    // Check if application has requested only the size.
    if (p_byte_array == NULL)
    {
        return ERROR_SUCCESS;
    }

    //Fetch bytes from the FIFO.
    while (index < write_size)
    {
        fifo_put(p_fifo, p_byte_array[index++]);
    }

    (*p_size) = write_size;

    return ERROR_SUCCESS;
}
