
void __cdecl16far auto_file_current_hst(void)

{
  undefined1 uVar1;
  int iVar2;
  undefined2 uVar3;
  int iVar4;
  undefined2 uVar5;
  uint unaff_DI;
  void *unaff_SS;
  char *pcVar6;
  undefined1 local_170 [220];
  undefined1 local_94 [16];
  undefined2 local_84 [8];
  undefined1 local_74 [100];
  undefined2 local_10;
  int local_e;
  undefined4 local_c;
  long local_8;
  int local_4;
  
  iVar2 = game_turn();
  if (iVar2 == 2) {
    iVar2 = 9;
    pcVar6 = get_file_string(6,0);
    iVar2 = open(pcVar6,iVar2);
    if (iVar2 < 0) {
      return;
    }
    local_8 = 4;
    write(iVar2,(void *)CONCAT22(4,unaff_SS),unaff_DI);
    close(iVar2);
  }
  else {
    iVar2 = game_turn();
    if (0xc9 < iVar2) {
      return;
    }
  }
  iVar2 = 2;
  pcVar6 = get_file_string(6,0);
  uVar5 = (undefined2)((ulong)pcVar6 >> 0x10);
  iVar2 = open(pcVar6,iVar2);
  read(iVar2,(void *)CONCAT22(4,unaff_SS),unaff_DI);
  close(iVar2);
  uVar3 = FUN_1d28_01d5((undefined2)local_8);
  local_c = CONCAT22(uVar5,uVar3);
  if (local_c != 0) {
    iVar2 = 1;
    pcVar6 = get_file_string(6,0);
    iVar2 = open(pcVar6,iVar2);
    read(iVar2,(void *)CONCAT22(4,unaff_SS),unaff_DI);
    read(iVar2,(void *)CONCAT22((undefined2)local_8,(void *)((ulong)local_c >> 0x10)),unaff_DI);
    FUN_1d28_050d((int)local_c,(int)((ulong)local_c >> 0x10));
    for (iVar4 = 0; iVar4 < city_count; iVar4 = iVar4 + 1) {
      if (*(char *)(*(byte *)(*(int *)(iVar4 * 0x41 + 0x157f) * 0xe0 +
                             *(int *)(iVar4 * 0x41 + 0x157d) * 2) + 0x710) == '\n') {
        uVar1 = *(undefined1 *)(iVar4 * 0x41 + 0x1592);
      }
      else {
        uVar1 = 0xff;
      }
      local_74[iVar4] = uVar1;
    }
    for (iVar4 = 0; iVar4 < 8; iVar4 = iVar4 + 1) {
      local_84[iVar4] = *(undefined2 *)(iVar4 * 0x14 + 0x185);
    }
    FUN_6ef3_0947(local_94);
    FUN_828e_03f9();
    local_10 = FUN_6d51_132f(local_170);
    local_4 = city_count + local_e + 0x34;
    iVar4 = city_count;
    if ((city_count != 0) && (city_count / 2 == (city_count + -1) / 2)) {
      iVar4 = city_count + 1;
      local_4 = city_count + local_e + 0x35;
    }
    write(iVar2,(void *)CONCAT22(2,unaff_SS),unaff_DI);
    write(iVar2,(void *)CONCAT22(0x10,unaff_SS),unaff_DI);
    write(iVar2,(void *)CONCAT22(0x10,unaff_SS),unaff_DI);
    write(iVar2,(void *)0x104125,unaff_DI);
    write(iVar2,(void *)CONCAT22(iVar4,unaff_SS),unaff_DI);
    write(iVar2,(void *)CONCAT22(2,unaff_SS),unaff_DI);
    write(iVar2,(void *)CONCAT22(local_e,unaff_SS),unaff_DI);
    local_8 = local_8 + local_4;
    FUN_170b_008c(iVar2,0,0,0);
    write(iVar2,(void *)CONCAT22(4,unaff_SS),unaff_DI);
    close(iVar2);
    FUN_6d51_1225();
  }
  return;
}


