
uint __cdecl16far FUN_57ea_0935(int param_1,undefined4 param_2)

{
  int iVar1;
  uint uVar2;
  undefined2 unaff_SI;
  int iVar3;
  char *unaff_SS;
  undefined1 local_e [5];
  byte abStack_9 [7];
  
  iVar1 = ai_neutral_neighbours
                    (param_1,(char *)CONCAT22(local_e,unaff_SS),(char *)CONCAT22(unaff_SI,unaff_SS))
  ;
  if (iVar1 == 0) {
LAB_57ea_097b:
    uVar2 = 0xff;
  }
  else {
    iVar1 = 6;
    do {
      iVar3 = iVar1 + -1;
      if (iVar1 == 0) goto LAB_57ea_097b;
      uVar2 = (uint)abStack_9[iVar1];
      iVar1 = iVar3;
    } while ((uVar2 == 0xff) || ('\x02' < *(char *)((int)param_2 + uVar2)));
  }
  return uVar2;
}


