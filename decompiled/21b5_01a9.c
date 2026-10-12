
bool __cdecl16far FUN_21b5_01a9(uint param_1)

{
  bool bVar1;
  int iVar2;
  int in_DX;
  undefined1 local_12 [16];
  
  if ((DAT_451b_2e40 < 0x1f) || ((param_1 & 0x39e) == 0)) {
    FUN_21b5_01f4(local_12);
    iVar2 = FUN_229d_0060(0x6d9c,0x4125,local_12);
    bVar1 = iVar2 != 0 || in_DX != 0;
  }
  else {
    bVar1 = false;
  }
  return bVar1;
}


