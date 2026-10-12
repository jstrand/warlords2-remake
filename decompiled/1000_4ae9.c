
int __cdecl16far dostounix(uint *param_1,undefined4 param_2)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  uint *puVar4;
  undefined2 uVar5;
  int local_6;
  
  tzset();
  iVar3 = DAT_4125_3f08 + -0x5a00;
  uVar1 = *param_1;
  iVar2 = __N_LXMUL();
  local_6 = __N_LXMUL();
  local_6 = iVar3 + iVar2 + local_6;
  if ((uVar1 - 0x7bc & 3) != 0) {
    local_6 = local_6 + 0x5180;
  }
  iVar3 = 0;
  uVar5 = (undefined2)((ulong)param_1 >> 0x10);
  puVar4 = (uint *)param_1;
  iVar2 = (int)*(char *)((int)puVar4 + 3);
  while (0 < iVar2 + -1) {
    iVar3 = iVar3 + *(char *)((int)(undefined2 *)&DAT_4125_3ed8 + iVar2);
    iVar2 = iVar2 + -1;
  }
  iVar3 = iVar3 + (char)puVar4[1] + -1;
  if (('\x02' < *(char *)((int)puVar4 + 3)) && ((*param_1 & 3) == 0)) {
    iVar3 = iVar3 + 1;
  }
  uVar5 = (undefined2)((ulong)param_2 >> 0x10);
  if (DAT_4125_3f0c != 0) {
    __isDST(*param_1 - 0x7b2,0,iVar3,*(undefined1 *)((int)param_2 + 1));
  }
  iVar2 = __N_LXMUL();
  iVar3 = __N_LXMUL();
  return local_6 + iVar2 + iVar3 + (uint)*(byte *)((int)param_2 + 3);
}


