
void __cdecl16far FUN_834b_05e4(int *param_1,int param_2)

{
  int iVar1;
  int *piVar2;
  undefined2 uVar3;
  int iVar4;
  int iVar5;
  undefined2 unaff_SS;
  undefined1 local_1e [8];
  undefined4 local_16;
  undefined4 local_12;
  int local_e;
  int local_c;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  piVar2 = (int *)param_1;
  uVar3 = param_1._2_2_;
  if (param_1 == (int *)0x0) {
    piVar2 = (int *)0x2a88;
    uVar3 = 0x4125;
  }
  __F_SCOPY(piVar2,uVar3,&local_e,unaff_SS);
  local_e = local_e + -3;
  local_a = local_a + 4;
  local_c = local_c + -3;
  local_8 = local_8 + 4;
  if (param_1 == (int *)0x0) {
    __F_SCOPY(0x2a88,0x4125,&local_16,unaff_SS);
    FUN_216d_002e(local_1e);
  }
  else {
    __F_SCOPY(param_1,local_1e,unaff_SS);
    local_16 = CONCAT22(((int *)param_1)[1] << 1,*param_1 << 1);
    local_12 = CONCAT22(((int *)param_1)[3] << 1,((int *)param_1)[2] << 1);
  }
  FUN_1997_034a();
  if (opt_hidden_map == 0) {
    (*DAT_451b_2a74)();
  }
  else {
    FUN_24d0_0497();
    (*DAT_451b_2a68)(0x24d0,0x581d,0x4125,local_16,local_12,0x6795,0x4125,local_16,0x54e3,0x4125,
                     local_16);
  }
  for (local_4 = 0; local_4 < site_count; local_4 = local_4 + 1) {
    iVar4 = *(int *)(local_4 * 0x1f + 0x811);
    iVar1 = *(int *)(local_4 * 0x1f + 0x813);
    if (((((local_e <= iVar4) && (iVar4 < local_e + local_a)) && (local_c <= iVar1)) &&
        ((iVar1 < local_c + local_8 &&
         ((opt_hidden_map == 0 || ((*(byte *)(iVar1 * 0x70 + iVar4) & 0x20) != 0)))))) &&
       ((*(uint *)(local_4 * 0x1f + 0x82e) & 1 << ((byte)current_player & 0x1f)) != 0)) {
      if (*(char *)(local_4 * 0x1f + 0x829) == '\x01') {
        local_6 = 2;
      }
      else if ((*(byte *)(iVar1 * 0xe0 + iVar4 * 2 + 1) & 0x40) == 0) {
        if (*(int *)(local_4 * 0x1f + 0x82c) == 0) {
          local_6 = 0;
        }
        else {
          local_6 = 3;
        }
      }
      else {
        local_6 = 1;
      }
      iVar5 = iVar1 * 2 + -1;
      iVar4 = (iVar4 * 2 + 3 >> 3) * 8;
      FUN_1997_034a();
      FUN_1997_060a();
      (*DAT_451b_2a68)(0x1997,0x57a7,0x4125,*(undefined2 *)(local_6 * 8 + 0xf28),
                       *(undefined2 *)(local_6 * 8 + 0xf2a),*(undefined2 *)(local_6 * 8 + 0xf2c),
                       *(undefined2 *)(local_6 * 8 + 0xf2e),0x6795,0x4125,iVar4,iVar5,0x576c,0x4125,
                       *(undefined2 *)(local_6 * 8 + 0xf28),*(undefined2 *)(local_6 * 8 + 0xf2a));
      if (local_4 == param_2) {
        FUN_2012_0bb7(0x6795,0x4125,iVar4,iVar5,0xf000c);
        FUN_2012_0bb7(0x6795,0x4125,iVar4,iVar1 * 2 + 8,0xf000c);
        FUN_2012_0f08(0x6795,0x4125,iVar4,iVar5,0xf000a);
        FUN_2012_0f08();
      }
      FUN_1997_06bb();
    }
  }
  return;
}


