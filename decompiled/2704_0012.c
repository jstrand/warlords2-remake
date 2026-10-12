
void __cdecl16near FUN_2704_0012(void)

{
  uint *puVar1;
  uint *puVar2;
  int iVar3;
  uint *in_BX;
  uint *unaff_SI;
  uint *unaff_DI;
  undefined2 unaff_ES;
  
  if ((((unaff_SI[3] & 0x7fff) == 0 && *unaff_SI == 0) && unaff_SI[1] == 0) && unaff_SI[2] == 0) {
    if (in_BX == (uint *)0xffff) {
      return;
    }
    if ((int)in_BX[4] < 0x4001) {
      return;
    }
    if ((((in_BX[3] & 0x7fff) == 0 && *in_BX == 0) && in_BX[1] == 0) && in_BX[2] == 0) {
      return;
    }
  }
  else if (((((in_BX == (uint *)0xffff) || ((int)in_BX[4] < 0x4001)) ||
            ((((in_BX[3] & 0x7fff) == 0 && *in_BX == 0) && in_BX[1] == 0) && in_BX[2] == 0)) ||
           (in_BX[3] < unaff_SI[3])) ||
          ((in_BX[3] <= unaff_SI[3] &&
           ((in_BX[2] < unaff_SI[2] ||
            ((in_BX[2] <= unaff_SI[2] &&
             ((in_BX[1] < unaff_SI[1] || ((in_BX[1] <= unaff_SI[1] && (*in_BX < *unaff_SI)))))))))))
          ) goto LAB_2704_0086;
  unaff_SI = in_BX;
LAB_2704_0086:
  FUN_2704_2714();
  for (iVar3 = 6; iVar3 != 0; iVar3 = iVar3 + -1) {
    puVar2 = unaff_DI;
    unaff_DI = unaff_DI + 1;
    puVar1 = unaff_SI;
    unaff_SI = unaff_SI + 1;
    *puVar2 = *puVar1;
  }
  return;
}


