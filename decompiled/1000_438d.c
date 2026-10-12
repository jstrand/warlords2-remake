
void * __cdecl16far memset(void *dst,int c,uint n)

{
  setmem(dst,n,(char)c);
  return dst;
}


