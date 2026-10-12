
time_t * __cdecl16far time(time_t *__return_storage_ptr__,time_t *__timer)

{
  time_t *ptVar1;
  undefined2 in_DX;
  undefined1 local_a [4];
  undefined1 local_6 [4];
  
  __getdate_raw(local_6);
  __gettime_raw(local_a);
  ptVar1 = (time_t *)dostounix(local_6);
  if (__return_storage_ptr__ != (time_t *)0x0 || __timer != (time_t *)0x0) {
    *(undefined2 *)((int)__return_storage_ptr__ + 2) = in_DX;
    *(time_t **)___return_storage_ptr__ = ptVar1;
  }
  return ptVar1;
}


