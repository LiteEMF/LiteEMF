/* 
*   BSD 2-Clause License
*   Copyright (c) 2022, LiteEMF
*   All rights reserved.
*   This software component is licensed by LiteEMF under BSD 2-Clause license,
*   the "License"; You may not use this file except in compliance with the
*   License. You may obtain a copy of the License at:
*       opensource.org/licenses/BSD-2-Clause
* 
*/

/************************************************************************************************************
**	Description:	
************************************************************************************************************/
#include  "hw_config.h"
#if 0
#include  "pb_decode.h"
#include  "pb_encode.h"
#include  "simple.pb.h"

#include "api/api_log.h"
/******************************************************************************************************
** Defined
*******************************************************************************************************/

/******************************************************************************************************
**	public Parameters
*******************************************************************************************************/


/******************************************************************************************************
**	static Parameters
*******************************************************************************************************/

/*****************************************************************************************************
**	static Function
******************************************************************************************************/

/*****************************************************************************************************
**  Function
******************************************************************************************************/

/*******************************************************************
** Parameters:		
** Returns:	
** Description:		
*******************************************************************/
bool protobuf_test(void)
{
	person_t persion = person_t_init_default;
	person_t unpack_persion = person_t_init_default;
	uint8_t buffer[person_t_size] = {0};
	pb_ostream_t o_stream = PB_OSTREAM_SIZING;
	pb_istream_t i_stream = PB_ISTREAM_EMPTY;

	memset(buffer,0,sizeof(buffer));
	// 组包
	persion.student_count=1;
    persion.student[0].id = 0x01;
	persion.student[0].array8.size = 2;
	persion.student[0].array8.bytes[0] = 0x43;
	persion.student[0].array8.bytes[1] = 0x45;
	memset(persion.student[0].fix_array8,0x10,sizeof(persion.student[0].fix_array8));
	persion.student[0].array32_count = 2;
	memset(persion.student[0].array32,0x20,sizeof(uint32_t)*persion.student[0].array32_count);
	memcpy(persion.student[0].str,"Hellow!",sizeof("Hellow!"));
	    bool has_teacher;
    teacher_t teacher;

	persion.has_teacher = true;
	persion.teacher.id = 0x01;
	persion.teacher.array8.size = 2;
	persion.teacher.array8.bytes[0] = 0x53;
	persion.teacher.array8.bytes[1] = 0x55;
	memset(persion.teacher.fix_array8,0x10,sizeof(persion.teacher.fix_array8));
	persion.teacher.array32_count = 1;
	memset(persion.teacher.array32,0x40,sizeof(uint32_t)*persion.teacher.array32_count);
	memcpy(persion.teacher.str, "world!",sizeof("world!"));

	// encode
	o_stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
	pb_encode(&o_stream, person_t_fields, &persion);
	logd("ostream max_size=%d, bytes_written=%d\n",o_stream.max_size,o_stream.bytes_written);
	dump(buffer, o_stream.bytes_written);
	// decode
	i_stream = pb_istream_from_buffer(buffer, sizeof(buffer));
	pb_decode(&i_stream, person_t_fields, &unpack_persion);

	logd("sizeof(person_t)=%d\n",sizeof(person_t));
	logd("sizeof(student_t)=%d\n",sizeof(student_t));
	logd("sizeof(teacher_t)=%d\n",sizeof(teacher_t));
	if(0 == memcmp(&persion, &unpack_persion,sizeof(persion))){
		loge("nonapb test ok...\n");
		logd("id:%d\n",unpack_persion.student[0].id);
		logd("array8:%d:",unpack_persion.student[0].array8.size);
		dump(unpack_persion.student[0].array8.bytes,unpack_persion.student[0].array8.size);
	}else{
		loge("nonapb test faile...\n");
		logd("persion %d:\n",sizeof(persion));dump(&persion,sizeof(persion));
		logd("unpack_persion %d:\n",sizeof(unpack_persion));dump(&unpack_persion,sizeof(unpack_persion));
	}

	return true;
}

#endif






