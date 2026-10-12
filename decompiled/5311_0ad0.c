
undefined2 __cdecl16far FUN_5311_0ad0(int param_1,int param_2,int *param_3)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  
  iVar3 = 8;
  do {
    if (iVar3 == 0) {
      return 0;
    }
    iVar1 = dice(1,0xc,-1);
    uVar4 = (undefined2)((ulong)param_3 >> 0x10);
    piVar2 = (int *)param_3;
    *(undefined4 *)param_3 = *(undefined4 *)(iVar1 * 4 + 0x382);
    *param_3 = *param_3 + param_1;
    piVar2[1] = piVar2[1] + param_2;
    uVar5 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
    iVar1 = (int)DAT_451b_002a;
  } while (((*(char *)(iVar1 + piVar2[1] * 0x70 + *param_3 + 0x6120) != '\a') &&
           (*(char *)(iVar1 + piVar2[1] * 0x70 + *param_3 + 0x6120) != '\x04')) &&
          (iVar3 = iVar3 + -1, *(char *)(iVar1 + piVar2[1] * 0x70 + *param_3 + 0x6120) != '\x05'));
  return 1;
}


