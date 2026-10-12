
void __cdecl16far FUN_8065_0f3f(void)

{
  int *piVar1;
  undefined2 uVar2;
  int iVar3;
  int iVar4;
  
  piVar1 = (int *)DAT_451b_1f02;
  uVar2 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
  if (DAT_451b_1f02 == (int *)0x0) {
LAB_8065_0fc8:
    if (DAT_451b_1f02 == (int *)0x0) goto LAB_8065_0fe3;
LAB_8065_0fd0:
    iVar4 = piVar1[1];
    iVar3 = *DAT_451b_1f02;
  }
  else {
    if ((piVar1[9] < 0) || (piVar1[10] < 0)) goto LAB_8065_0fc8;
    if ((((*DAT_451b_1f02 < DAT_3c04_0177) || (DAT_3c04_017b < *DAT_451b_1f02)) ||
        (piVar1[1] < DAT_3c04_0175)) ||
       ((DAT_3c04_0179 < piVar1[1] ||
        ((piVar1[9] == DAT_3c04_017d && (piVar1[10] == DAT_3c04_017f)))))) goto LAB_8065_0fd0;
    iVar4 = piVar1[10];
    iVar3 = piVar1[9];
  }
  FUN_8611_0629(iVar3,iVar4);
LAB_8065_0fe3:
  FUN_8065_0174();
  return;
}


