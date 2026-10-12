
double * __cdecl16far sqrt(double *__return_storage_ptr__,double __x)

{
  double *pdVar1;
  int in_stack_0000000a;
  
  pdVar1 = (double *)(in_stack_0000000a << 1);
  if ((pdVar1 != (double *)0x0) && (in_stack_0000000a < 0)) {
    pdVar1 = (double *)__matherr_raise(1,0x3a48,0x4125,&__return_storage_ptr__);
  }
  return pdVar1;
}


