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

#include "hw_config.h"
#include "utils/list.h"
#include "utils/emf_utils.h"

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
typedef struct {
    uint8_t val;
    uint8_t other;
    list_head_t list;
}student_list_t;

void list_test(void){

    student_list_t *n;
    student_list_t *studentp;
    uint8_t i;

    //list head init two method
    // struct list_head mylist;
    // INIT_LIST_HEAD(&mylist);
    struct list_head mylist = LIST_HEAD_INIT(mylist);

    //增加
    logd("\nlist add entry test:\n");
    for(i = 0; i < 5; i++){
        studentp = emf_malloc(sizeof(student_list_t));
        studentp->val = i;
        logd("add to list:%x\n",studentp->val);
        // list_add_tail(&studentp->list, &mylist);     //增加在尾端
        list_add(&(studentp->list), &mylist);
    }

    //遍历
    logd("\nlist for each entry test:\n");
    list_for_each_entry(studentp, &mylist, student_list_t, list){
        logd("list entry:%x\n", studentp->val);
    }

    //delay
    logd("\nlist delay test:\n");
    list_for_each_entry_safe(studentp, n, &mylist, student_list_t, list){
        if(studentp->val % 2){
            logd("list delay:%x\n", studentp->val);
            list_del(&studentp->list);
            emf_free(studentp);
        }
    }

    //重新遍历
    logd("list for each entry:\n");
    list_for_each_entry(studentp, &mylist, student_list_t, list){
        logd("list entry:%x\n", studentp->val);
    }
}

