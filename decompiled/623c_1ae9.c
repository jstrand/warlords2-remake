
int __cdecl16far FUN_623c_1ae9(int param_1)

{
  int iVar1;
  int iVar2;
  int local_6;
  int local_4;
  
  local_4 = -1;
  local_6 = 10;
  iVar2 = 0;
  while ((iVar1 = -1, iVar2 < 4 && (iVar1 = iVar2, *(char *)(param_1 * 0x41 + iVar2 + 0x1593) != -1)
         )) {
    iVar1 = (int)*(char *)(param_1 * 0x41 + iVar2 + 0x159b);
    if (((undefined1 *)&DAT_451b_0669)[*(char *)(param_1 * 0x41 + iVar2 + 0x1593) * 6] == '\x01') {
      iVar1 = iVar1 + 2;
    }
    if (iVar1 < local_6) {
      local_6 = iVar1;
      local_4 = iVar2;
    }
    iVar2 = iVar2 + 1;
  }
  if (iVar1 == -1) {
    iVar1 = local_4;
  }
  return iVar1;
}


