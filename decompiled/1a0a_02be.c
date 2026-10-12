
void __cdecl16far FUN_1a0a_02be(undefined4 param_1)

{
  char cVar1;
  undefined2 uVar2;
  int in_DX;
  int iVar3;
  undefined2 uVar4;
  int local_e;
  int local_c;
  undefined1 local_a [8];
  
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  iVar3 = (int)param_1;
  FUN_21e2_0ab8(*(undefined4 *)(iVar3 + 0xc),&local_c);
  if ((*(char *)(iVar3 + 10) == '\0') ||
     (in_DX = local_c + 2, (int)(uint)*(byte *)(iVar3 + 10) < in_DX)) {
    *(char *)(iVar3 + 10) = (char)local_c + '\x04';
  }
  if ((*(char *)(iVar3 + 0xb) == '\0') ||
     (in_DX = local_e + 2, (int)(uint)*(byte *)(iVar3 + 0xb) < in_DX)) {
    *(char *)(iVar3 + 0xb) = (char)local_e + '\x05';
  }
  FUN_24d0_0002();
  uVar2 = FUN_216d_002e(local_a);
  FUN_216d_0288(uVar2,in_DX);
  cVar1 = *(char *)(iVar3 + 4);
  if (cVar1 == '\0') {
    FUN_78a8_06ae(0xf0000,0x30000);
  }
  else if (cVar1 == '\x01') {
    FUN_78a8_06ae(0xf0000,0x30000);
  }
  else {
    if (cVar1 != '\x02') goto LAB_1a0a_03ed;
    FUN_78a8_06ae(0x20000,0x30000);
  }
  FUN_24d0_02e5(local_a);
LAB_1a0a_03ed:
  FUN_21e2_0401(*(int *)(iVar3 + 6) + (int)((uint)*(byte *)(iVar3 + 10) - local_c) / 2,
                *(int *)(iVar3 + 8) + (int)((uint)*(byte *)(iVar3 + 0xb) - local_e) / 2 + 1,
                *(undefined4 *)(iVar3 + 0xc));
  return;
}


