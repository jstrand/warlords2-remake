
int __cdecl16far rand(void)

{
  long lVar1;
  
  lVar1 = __N_LXMUL();
  DAT_4125_3d86 = (uint)((ulong)(lVar1 + 1) >> 0x10);
  rand_seed = (int)(lVar1 + 1);
  return DAT_4125_3d86 & 0x7fff;
}


