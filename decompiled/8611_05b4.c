
void __cdecl16far FUN_8611_05b4(void)

{
  int iVar1;
  int iVar2;
  
  iVar2 = DAT_3c04_017f;
  iVar1 = DAT_3c04_017d;
  if (DAT_451b_1f02 != (int *)0x0) {
    iVar2 = ((int *)DAT_451b_1f02)[1];
    iVar1 = *DAT_451b_1f02;
  }
  if ((((iVar1 < DAT_3c04_0177) || (DAT_3c04_017b < iVar1)) || (iVar2 < DAT_3c04_0175)) ||
     (DAT_3c04_0179 < iVar2)) {
    FUN_8611_0629(iVar1,iVar2);
  }
  else {
    FUN_8611_006c();
  }
  return;
}


