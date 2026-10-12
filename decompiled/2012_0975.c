
undefined2 __cdecl16far FUN_2012_0975(undefined4 param_1)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  iVar1 = (int)param_1;
  if ((*(uint *)(iVar1 + 0x35) & 2) == 0) {
    for (iVar2 = 0; iVar2 < (int)(uint)*(byte *)(iVar1 + 0x34); iVar2 = iVar2 + 1) {
      FUN_1d28_048e(*(undefined4 *)(iVar1 + iVar2 * 4 + 10));
    }
  }
  else {
    auto_str_palloc_no_hdr();
  }
  *(undefined1 *)(iVar1 + 0x34) = 0;
  return 1;
}


