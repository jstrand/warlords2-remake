
void __cdecl16far
FUN_7204_1f06(undefined2 param_1,undefined2 param_2,char *param_3,char *param_4,char *param_5)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 in_DX;
  int iVar3;
  int iVar4;
  undefined2 unaff_SS;
  char *pcVar5;
  undefined2 uVar6;
  char local_c [10];
  
  FUN_7204_1e02(param_1);
  uVar1 = mem_lock((int)DAT_451b_0524,(int)((ulong)DAT_451b_0524 >> 0x10));
  DAT_451b_0520 = CONCAT22(in_DX,uVar1);
  iVar4 = 0;
  sprintf((char *)CONCAT22(unaff_SS,local_c),(char *)0x41250ffe,param_2);
  for (iVar3 = 0; iVar3 < 8000; iVar3 = iVar3 + 1) {
    uVar1 = (undefined2)((ulong)DAT_451b_0520 >> 0x10);
    if ((*(char *)((int)DAT_451b_0520 + iVar3) == '#') &&
       (iVar2 = strcmp((char *)CONCAT22(uVar1,(char *)(iVar3 + (int)DAT_451b_0520 + 1)),
                       (char *)CONCAT22(unaff_SS,local_c)), iVar2 == 0)) goto LAB_7204_1ff0;
  }
LAB_7204_2002:
  mem_unlock((int)DAT_451b_0524,(int)((ulong)DAT_451b_0524 >> 0x10));
  return;
LAB_7204_1ff0:
  if (7999 < iVar3) goto LAB_7204_2002;
  uVar1 = (undefined2)((ulong)DAT_451b_0520 >> 0x10);
  iVar2 = (int)DAT_451b_0520;
  if ((*(char *)(iVar2 + iVar3) == '\0') && (iVar4 == 0)) {
    pcVar5 = (char *)param_3;
    uVar6 = (undefined2)((ulong)param_3 >> 0x10);
LAB_7204_1f96:
    strcpy((char *)CONCAT22(uVar6,pcVar5),(char *)CONCAT22(uVar1,(char *)(iVar3 + iVar2 + 1)));
    iVar4 = iVar4 + 1;
  }
  else {
    if ((*(char *)(iVar2 + iVar3) == '\0') && (iVar4 == 1)) {
      pcVar5 = (char *)param_4;
      uVar6 = (undefined2)((ulong)param_4 >> 0x10);
      goto LAB_7204_1f96;
    }
    if ((*(char *)(iVar2 + iVar3) == '\0') && (iVar4 == 2)) {
      strcpy(param_5,(char *)CONCAT22(uVar1,(char *)(iVar3 + iVar2 + 1)));
      goto LAB_7204_2002;
    }
  }
  iVar3 = iVar3 + 1;
  goto LAB_7204_1ff0;
}


