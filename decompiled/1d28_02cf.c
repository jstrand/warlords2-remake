
void __cdecl16near FUN_1d28_02cf(void)

{
  uint uVar1;
  undefined2 uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  int unaff_SI;
  undefined2 uVar8;
  
  uVar8 = 0x4125;
  iVar3 = DAT_4125_327a;
LAB_1d28_02d2:
  do {
    if (iVar3 == *(int *)&DAT_4125_327e) {
LAB_1d28_0487:
      *(undefined2 *)&DAT_4125_327c = *(undefined2 *)&DAT_4125_327a;
      return;
    }
    iVar7 = iVar3;
    if ((((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] & 0x80U) == 0) {
      while( true ) {
        iVar3 = iVar7;
        if ((iVar3 == *(int *)&DAT_4125_327e) ||
           (iVar7 = iVar3 + *(int *)&DAT_4125_0002, iVar7 == *(int *)&DAT_4125_327e))
        goto LAB_1d28_0487;
        if ((((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] & 0x20U) != 0) break;
        uVar2 = *(undefined2 *)&DAT_4125_0002;
        FUN_1d28_0be7();
        *(int *)(*(int *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 2) = iVar3 + 1;
        iVar7 = iVar3 + *(int *)&DAT_4125_0002;
        *(int *)0x0 = iVar3;
        *(undefined2 *)&DAT_4125_0002 = uVar2;
        ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] = '\0';
        iVar4 = iVar7 + *(int *)&DAT_4125_0002;
        unaff_SI = iVar3;
        if (iVar4 != *(int *)&DAT_4125_327e) {
          if ((((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] & 0x80U) == 0) {
            *(int *)&DAT_4125_0002 = *(int *)&DAT_4125_0002 + *(int *)&DAT_4125_0002;
            iVar3 = iVar7 + *(int *)&DAT_4125_0002;
            if (iVar3 != *(int *)&DAT_4125_327e) {
              *(int *)0x0 = iVar7;
            }
          }
          else {
            *(int *)0x0 = iVar7;
          }
        }
      }
      while( true ) {
        uVar6 = 0;
        iVar7 = iVar3;
        while (iVar7 = iVar7 + *(int *)&DAT_4125_0002, iVar7 != *(int *)&DAT_4125_327e) {
          if ((((((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] & 0xa0U) == 0x80) &&
              (uVar1 = *(uint *)&DAT_4125_0002, uVar1 <= *(uint *)&DAT_4125_0002)) &&
             (uVar6 <= uVar1)) {
            uVar6 = uVar1;
            unaff_SI = iVar7;
          }
        }
        if (uVar6 == 0) {
          iVar3 = iVar3 + *(int *)&DAT_4125_0002;
          goto LAB_1d28_02d2;
        }
        iVar7 = *(int *)&DAT_4125_0002;
        iVar4 = *(int *)&DAT_4125_0002;
        FUN_1d28_0be7(*(undefined2 *)&DAT_4125_0002,uVar8,unaff_SI);
        unaff_SI = iVar3 + 1;
        *(int *)(*(int *)s_Borland_C_____Copyright_1991_Bor_4125_0004 + 2) = unaff_SI;
        FUN_1d28_060d();
        if (iVar7 == iVar4) goto LAB_1d28_02d2;
        if (iVar7 + 1 == iVar4) break;
        iVar5 = iVar3 + iVar7;
        *(int *)0x0 = iVar3;
        *(int *)&DAT_4125_0002 = iVar4 - iVar7;
        ((char *)s_Borland_C_____Copyright_1991_Bor_4125_0004)[4] = '\0';
        *(int *)0x0 = iVar5;
        iVar3 = iVar5;
      }
      *(int *)&DAT_4125_0002 = *(int *)&DAT_4125_0002 + 1;
      goto LAB_1d28_02d2;
    }
    iVar3 = iVar3 + *(int *)&DAT_4125_0002;
  } while( true );
}


