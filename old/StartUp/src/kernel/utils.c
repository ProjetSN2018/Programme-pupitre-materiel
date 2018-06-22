/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "utils.h"
#include <stdlib.h>

char _IsSeparator(char ch, const char*pSep);
static char     _chEndToken  = '\0';
static char*    _pEndToken = NULL;

char* mstrtok(char*pStr,const char*pSep)
{
    static char*pToken=NULL;
    
    if(pStr) 
    {
        pToken=pStr;
    }
    else if(_pEndToken)
    {
        *_pEndToken=_chEndToken;
        pToken=_pEndToken;
    }
    else
    {
        return NULL;
    }
    
    while(*pToken && _IsSeparator(*pToken,pSep)) { pToken++; }
    _pEndToken=pToken;
    while(*_pEndToken && !_IsSeparator(*_pEndToken,pSep)) { _pEndToken++; }
    _chEndToken=*_pEndToken;
    *_pEndToken='\0';

    return pToken;
}

void  mstrtokRestore(void)
{
    if(_pEndToken) *_pEndToken=_chEndToken;
}

void  mstrtokReset(void)
{
    _chEndToken  = '\0';
    _pEndToken = NULL;
}

char _IsSeparator(char ch, const char*pSep)
{
    while(*pSep && (*pSep!=ch)) pSep++;
    return *pSep;
}