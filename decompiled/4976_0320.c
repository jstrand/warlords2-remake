
void __cdecl16far auto_ui_quest_pillage_gold(void)

{
  uint *puVar1;
  char cVar2;
  undefined4 uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  undefined2 unaff_SS;
  char *pcVar6;
  int iVar7;
  int iVar8;
  char local_64 [80];
  undefined1 local_14 [2];
  uint local_12;
  int local_10;
  int iStack_e;
  undefined2 uStack_c;
  undefined2 uStack_a;
  int local_8;
  undefined2 uStack_6;
  undefined2 uStack_4;
  
  local_8 = 1;
  FUN_78a8_06ae(2,0,7,1);
  __F_SCOPY(current_player * 0xc + 0x1103,0x3c04,local_14,unaff_SS);
  sprintf((char *)CONCAT22(unaff_SS,local_64),s__s_s_Quest_4125_00ba,
          *(char *)(local_10 * 0x16 + -0x7776) * 0x14 + 0x223,0x3c04);
  FUN_7ecb_00d6(0x1b8,0x91,local_64);
  FUN_24d0_0002(0);
  FUN_2133_02fe(0x168,0xa5,0xa0);
  if (6 < local_12) {
    return;
  }
                    /* WARNING: Switch is manually overridden */
  switch((undefined4)*(undefined2 *)(local_12 * 2 + 0xd57)) {
  case 0x49b2d:
    pcVar6 = get_string(0x15,0);
    FUN_7ecb_00d6(0x1b8,0xaf,pcVar6);
    pcVar6 = get_string(0x15,1);
    FUN_7ecb_00d6(0x1b8,0xc3,pcVar6);
    iVar7 = *(char *)(iStack_e * 0x16 + -0x777b) * 0x14;
    uVar4 = 0x3c04;
    pcVar6 = get_string(0x15,2);
    sprintf((char *)CONCAT22(unaff_SS,local_64),pcVar6,iVar7,uVar4);
    FUN_7ecb_00d6(0x1b8,0xd7,local_64);
    FUN_7ecb_00d6(0x1b8,0xeb,*(char *)(iStack_e * 0x16 + -0x7776) * 0x14 + 0x223,0x3c04);
    pcVar6 = get_string(0x15,3);
    FUN_7ecb_00d6(0x1b8,0x109,pcVar6);
    iVar7 = FUN_828e_0b51(*(undefined2 *)(local_10 * 0x16 + -0x7780),
                          *(undefined2 *)(local_10 * 0x16 + -0x777e),
                          *(undefined2 *)(iStack_e * 0x16 + -0x7780),
                          *(undefined2 *)(iStack_e * 0x16 + -0x777e));
    uVar3 = *(undefined4 *)(iVar7 * 4 + 0x2b7c);
    FUN_7ecb_00d6(0x1b8,0x11d,(int)uVar3,(int)((ulong)uVar3 >> 0x10));
    FUN_828e_0a3f(*(undefined2 *)(local_10 * 0x16 + -0x7780),
                  *(undefined2 *)(local_10 * 0x16 + -0x777e),
                  *(undefined2 *)(iStack_e * 0x16 + -0x7780),
                  *(undefined2 *)(iStack_e * 0x16 + -0x777e),8);
    FUN_828e_0899(iStack_e * 0x16 + -0x7780,0x2e1d,8);
    FUN_828e_08b3(iStack_e * 0x16 + -0x7780,0x2e1d,8);
    goto LAB_4976_0583;
  case 0x49cf1:
    pcVar6 = get_string(0x16,0);
    FUN_7ecb_00d6(0x1b8,0xaf,pcVar6);
    pcVar6 = get_string(0x16,1);
    FUN_7ecb_00d6(0x1b8,0xc3,pcVar6);
    FUN_7ecb_00d6(0x1b8,0xd7,iStack_e * 0x1d + 0xce9,0x3c04);
    pcVar6 = get_string(0x16,2);
    FUN_7ecb_00d6(0x1b8,0xf5,pcVar6);
    FUN_66d4_0b36(iStack_e,&uStack_4);
    iVar7 = FUN_828e_0b51(*(undefined2 *)(local_10 * 0x16 + -0x7780),
                          *(undefined2 *)(local_10 * 0x16 + -0x777e),uStack_4,uStack_6);
    uVar3 = *(undefined4 *)(iVar7 * 4 + 0x2b7c);
    FUN_7ecb_00d6(0x1b8,0x109,(int)uVar3,(int)((ulong)uVar3 >> 0x10));
    cVar2 = *(char *)(iStack_e * 0x1d + 0xcff);
    if (cVar2 != '\x01') {
      if (cVar2 == '\x02') {
        puVar1 = (uint *)(*(int *)(iStack_e * 0x1d + 0xd00) * 0x1f + 0x82e);
        *puVar1 = *puVar1 | 1 << ((byte)current_player & 0x1f);
      }
      else if (cVar2 != '\x03') {
        return;
      }
    }
    FUN_828e_0a3f(*(undefined2 *)(local_10 * 0x16 + -0x7780),
                  *(undefined2 *)(local_10 * 0x16 + -0x777e),uStack_4,uStack_6,8);
    FUN_828e_08cd(uStack_4,uStack_6,8);
    FUN_828e_099e(uStack_4,uStack_6,8);
    goto LAB_4976_0583;
  case 0x49e5e:
    pcVar6 = get_string(0x17,0);
    FUN_7ecb_00d6(0x1b8,0xaf,pcVar6);
    pcVar6 = get_string(0x17,1);
    FUN_7ecb_00d6(0x1b8,0xc3,pcVar6);
    pcVar6 = get_string(0x17,2);
    uVar5 = (undefined2)((ulong)pcVar6 >> 0x10);
    FUN_7ecb_00d6(0x1b8,0xd7,pcVar6);
    uVar4 = FUN_7087_14f4(iStack_e);
    sprintf((char *)CONCAT22(unaff_SS,local_64),(char *)0x412500c5,uVar4,uVar5);
    FUN_7ecb_00d6(0x1b8,0xeb,local_64);
    iVar7 = -1;
    break;
  case 0x49ee8:
    pcVar6 = get_string(0x18,0);
    FUN_7ecb_00d6(0x1b8,0xaf,pcVar6);
    pcVar6 = get_string(0x18,1);
    sprintf((char *)CONCAT22(unaff_SS,local_64),pcVar6,uStack_c);
    FUN_7ecb_00d6(0x1b8,0xc3,local_64);
    FUN_7ecb_00d6(0x1b8,0xd7,iStack_e * 0x14,0x3c04);
    pcVar6 = get_string(0x18,2);
    FUN_7ecb_00d6(0x1b8,0xf5,pcVar6);
    pcVar6 = get_string(0x18,3);
    sprintf((char *)CONCAT22(unaff_SS,local_64),pcVar6,uStack_a);
    FUN_7ecb_00d6(0x1b8,0x109,local_64);
    iVar7 = iStack_e;
    iStack_e = -1;
    break;
  case 0x49fb0:
    if (((opt_hidden_map != 0) &&
        ((*(byte *)(*(int *)(iStack_e * 0x41 + 0x157f) * 0x70 + *(int *)(iStack_e * 0x41 + 0x157d))
         & 0x20) == 0)) &&
       (((*(byte *)(*(int *)(iStack_e * 0x41 + 0x157f) * 0x70 + *(int *)(iStack_e * 0x41 + 0x157d))
         & 0x20) == 0 &&
        (((*(byte *)(*(int *)(iStack_e * 0x41 + 0x157f) * 0x70 + *(int *)(iStack_e * 0x41 + 0x157d)
                    + 0x71) & 0x20) == 0 &&
         ((*(byte *)(*(int *)(iStack_e * 0x41 + 0x157f) * 0x70 + *(int *)(iStack_e * 0x41 + 0x157d)
                    + 0x70) & 0x20) == 0)))))) {
      local_8 = 0;
    }
    pcVar6 = get_string(0x19,0);
    FUN_7ecb_00d6(0x1b8,0xaf,pcVar6);
    if (local_8 == 0) {
      iVar7 = 2;
    }
    else {
      iVar7 = 1;
    }
    iVar8 = iStack_e * 0x41 + 0x1581;
    uVar4 = 0x3c04;
    pcVar6 = get_string(0x19,iVar7);
    sprintf((char *)CONCAT22(unaff_SS,local_64),pcVar6,iVar8,uVar4);
    FUN_7ecb_00d6(0x1b8,0xc3,local_64);
    pcVar6 = get_string(0x19,3);
    FUN_7ecb_00d6(0x1b8,0xd7,pcVar6);
    pcVar6 = get_string(0x19,4);
    FUN_7ecb_00d6(0x1b8,0xeb,pcVar6);
    iVar7 = 0x19;
    goto LAB_4976_0ba1;
  case 0x4a15a:
    if ((((opt_hidden_map != 0) &&
         ((*(byte *)(*(int *)(iStack_e * 0x41 + 0x157f) * 0x70 + *(int *)(iStack_e * 0x41 + 0x157d))
          & 0x20) == 0)) &&
        ((*(byte *)(*(int *)(iStack_e * 0x41 + 0x157f) * 0x70 + *(int *)(iStack_e * 0x41 + 0x157d))
         & 0x20) == 0)) &&
       (((*(byte *)(*(int *)(iStack_e * 0x41 + 0x157f) * 0x70 + *(int *)(iStack_e * 0x41 + 0x157d) +
                   0x71) & 0x20) == 0 &&
        ((*(byte *)(*(int *)(iStack_e * 0x41 + 0x157f) * 0x70 + *(int *)(iStack_e * 0x41 + 0x157d) +
                   0x70) & 0x20) == 0)))) {
      local_8 = 0;
    }
    pcVar6 = get_string(0x1a,0);
    FUN_7ecb_00d6(0x1b8,0xaf,pcVar6);
    if (local_8 == 0) {
      iVar7 = 2;
    }
    else {
      iVar7 = 1;
    }
    iVar8 = iStack_e * 0x41 + 0x1581;
    uVar4 = 0x3c04;
    pcVar6 = get_string(0x1a,iVar7);
    sprintf((char *)CONCAT22(unaff_SS,local_64),pcVar6,iVar8,uVar4);
    FUN_7ecb_00d6(0x1b8,0xc3,local_64);
    pcVar6 = get_string(0x1a,3);
    FUN_7ecb_00d6(0x1b8,0xd7,pcVar6);
    pcVar6 = get_string(0x1a,4);
    FUN_7ecb_00d6(0x1b8,0xeb,pcVar6);
    iVar7 = 0x1a;
LAB_4976_0ba1:
    pcVar6 = get_string(iVar7,5);
    FUN_7ecb_00d6(0x1b8,0x109,pcVar6);
    iVar7 = FUN_828e_0b51(*(undefined2 *)(local_10 * 0x16 + -0x7780),
                          *(undefined2 *)(local_10 * 0x16 + -0x777e),
                          *(undefined2 *)(iStack_e * 0x41 + 0x157d),
                          *(undefined2 *)(iStack_e * 0x41 + 0x157f));
    uVar3 = *(undefined4 *)(iVar7 * 4 + 0x2b7c);
    FUN_7ecb_00d6(0x1b8,0x11d,(int)uVar3,(int)((ulong)uVar3 >> 0x10));
    FUN_828e_0a3f(*(undefined2 *)(local_10 * 0x16 + -0x7780),
                  *(undefined2 *)(local_10 * 0x16 + -0x777e),
                  *(undefined2 *)(iStack_e * 0x41 + 0x157d),
                  *(undefined2 *)(iStack_e * 0x41 + 0x157f),8);
    FUN_828e_07bf(iStack_e,8);
    FUN_828e_0837(iStack_e,8);
    return;
  case 0x4a3e1:
    pcVar6 = get_string(0x1b,0);
    FUN_7ecb_00d6(0x1b8,0xaf,pcVar6);
    pcVar6 = get_string(0x1b,1);
    sprintf((char *)CONCAT22(unaff_SS,local_64),pcVar6,uStack_c);
    FUN_7ecb_00d6(0x1b8,0xc3,local_64);
    pcVar6 = get_string(0x1b,2);
    FUN_7ecb_00d6(0x1b8,0xd7,pcVar6);
    pcVar6 = get_string(0x1b,3);
    FUN_7ecb_00d6(0x1b8,0xeb,pcVar6);
    pcVar6 = get_string(0x1b,4);
    FUN_7ecb_00d6(0x1b8,0x109,pcVar6);
    pcVar6 = get_string(0x1b,5);
    sprintf((char *)CONCAT22(unaff_SS,local_64),pcVar6,uStack_a);
    FUN_7ecb_00d6(0x1b8,0x11d,local_64);
    return;
  }
  FUN_834b_158d(0,0,iVar7,iStack_e,1,0);
LAB_4976_0583:
  FUN_834b_04d3(0,0);
  return;
}


