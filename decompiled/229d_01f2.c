
void __cdecl16far FUN_229d_01f2(undefined4 param_1)

{
  int iVar1;
  undefined2 uVar2;
  
  for (iVar1 = 0; iVar1 < 5; iVar1 = iVar1 + 1) {
    ((undefined1 *)&DAT_4125_355a)[iVar1] = 0;
  }
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  iVar1 = (int)param_1;
  *(undefined4 *)(iVar1 + 10) = 0;
  *(undefined4 *)(iVar1 + 6) = 0;
  *(undefined2 *)(iVar1 + 4) = 0;
  return;
}


