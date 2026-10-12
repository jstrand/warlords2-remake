
void __cdecl16far ai_production(void)

{
  byte bVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int city;
  undefined2 uVar5;
  
  FUN_5f19_0aed();
  iVar4 = city_count;
  if (opt_hidden_map != 0) {
    FUN_57ea_02ff();
    iVar4 = city_count;
  }
LAB_5db9_083a:
  do {
    do {
      do {
        city = iVar4 + -1;
        if (iVar4 == 0) {
          return;
        }
        iVar4 = city;
      } while (*(char *)(city * 0x41 + 0x1592) != (char)current_player);
      *(undefined2 *)((undefined1 *)&LAB_4125_15ad_1 + city * 0x41) = 0;
      *(undefined2 *)((undefined1 *)&LAB_4125_15b0 + city * 0x41) = 0xffff;
      *(undefined2 *)(city * 0x41 + 0x15b2) = 0xffff;
      iVar2 = city_is_building();
    } while (iVar2 != 0);
    if ((*(int *)(current_player * 0x14 + 0x185) < 0x28) &&
       ((int)((undefined2 *)&side_income)[current_player] <
        (int)((undefined2 *)&side_upkeep)[current_player])) {
      return;
    }
    uVar5 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    iVar2 = (int)DAT_451b_018e;
    uVar3 = *(byte *)(iVar2 + city + 0x56) - 2;
    if (0xb < uVar3) goto override_jmp_5db9_07b2_case_7;
                    /* WARNING: Switch is manually overridden */
    switch((undefined4)*(undefined2 *)(uVar3 * 2 + 0x847)) {
    case 0x5e347:
      bVar1 = *(byte *)(iVar2 + city + 0x11e);
      goto joined_r0x0005e37a;
    case 0x5e36a:
      bVar1 = *(byte *)(iVar2 + city + 0x11e);
joined_r0x0005e37a:
      if ((bVar1 & 2) == 0) {
        ai_city_flag_check();
      }
LAB_5db9_07d0:
      iVar2 = 4;
      break;
    case 0x5e380:
      if (opt_neutral_cities != 0) goto override_jmp_5db9_07b2_case_2;
      iVar2 = 1;
      break;
    case 0x5e399:
override_jmp_5db9_07b2_case_2:
      iVar2 = 2;
      break;
    case 0x5e3a3:
    case 0x5e3b9:
override_jmp_5db9_07b2_case_7:
      iVar2 = 3;
      break;
    case 0x5e3a5:
      goto override_jmp_5db9_07b2_case_6;
    case 0x5e3b7:
      goto LAB_5db9_07d0;
    }
    best_production_for(city,iVar2,-1,0);
  } while( true );
override_jmp_5db9_07b2_case_6:
  *(undefined1 *)(city * 0x41 + 0x15aa) = 0;
  goto LAB_5db9_083a;
}


