
void __advance_cursor(int *param_1,int *param_2)

{
  byte bVar1;
  int extraout_DX;
  char cVar3;
  int iVar2;
  
  iVar2 = *param_2;
  if (iVar2 != *param_1) {
    __video_int();
    *param_1 = extraout_DX;
    iVar2 = extraout_DX;
  }
  cVar3 = (char)((uint)iVar2 >> 8);
  bVar1 = (char)iVar2 + 1;
  iVar2 = CONCAT11(cVar3,bVar1);
  if (DAT_4125_3e7a <= bVar1) {
    iVar2 = (uint)(byte)(cVar3 + 1) << 8;
  }
  *param_2 = iVar2;
  return;
}


