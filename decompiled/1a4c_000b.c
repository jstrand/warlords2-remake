
/* WARNING: Removing unreachable block (ram,0x0001a6c9) */
/* WARNING: Removing unreachable block (ram,0x0001a64a) */

void __cdecl16far main(int param_1,undefined4 param_2)

{
  char cVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 *puVar5;
  int *piVar6;
  undefined2 uVar7;
  undefined4 local_c;
  
  DAT_4125_3286 = 0x1a4c;
  DAT_4125_3284 = 0x388;
  iVar2 = FUN_1ea3_0000();
  if (iVar2 == -1) {
    printf(s_Sorry__not_enough_memory_to_load_4125_26ce);
    exit(1);
  }
  FUN_152a_0251();
  DAT_4125_3384 = 1;
  DAT_4125_338c = 0;
  if (1 < param_1) {
    for (iVar2 = 1; iVar2 < param_1; iVar2 = iVar2 + 1) {
      uVar7 = (undefined2)((ulong)param_2 >> 0x10);
      puVar5 = (undefined2 *)((int)param_2 + iVar2 * 4);
      local_c = (char *)CONCAT22(puVar5[1],(char *)*puVar5);
      while ((*local_c == '-' || (*local_c == '/'))) {
        local_c = (char *)CONCAT22(local_c._2_2_,(char *)local_c + 1);
      }
      iVar3 = tolower((int)*local_c);
      iVar4 = 0x10;
      piVar6 = (int *)((char *)s_Random_data_not_found_4125_0274 + 1);
      do {
        if (*piVar6 == iVar3) {
                    /* WARNING: Could not recover jumptable at 0x0001a577. Too many branches */
                    /* WARNING: Treating indirect jump as call */
          (*(code *)piVar6[0x10])();
          return;
        }
        piVar6 = piVar6 + 1;
        iVar4 = iVar4 + -1;
      } while (iVar4 != 0);
      puts(s__x__Use_extended_memory_4125_2703);
      puts(s__h__Do_not_use_expanded_or_exten_4125_271d);
      puts(s__s__Use_alternate_army_names_4125_2748);
      puts(s__m__Mono_version__for_laptop_use_4125_2765);
      puts(s__v__No_talking_advisor_4125_2786);
      puts(s__n__No_startup_animation_4125_279d);
      exit(1);
    }
  }
  FUN_25de_08ef();
  DAT_451b_1d7c = 4;
  FUN_1ea3_008c();
  FUN_2012_0043();
  FUN_251d_0009();
  load_string_files();
  cVar1 = sound_init();
  if (cVar1 == '\0') {
    printf(s_Can_t_init_sound_4125_27b7);
    auto_str_warlords_c_strategic_studies();
  }
  sound_options_load();
  auto_file_area_dat();
  auto_str_can_t_allocate_os();
  DAT_451b_1d7d = 1;
  FUN_1e8f_004b();
  FUN_1e8f_0093();
  FUN_1e8f_00f8();
  dice_seed();
  DAT_451b_1d7e = 6;
  auto_file_standard_pal();
  auto_err_error_cannot_open_file();
  auto_file_standard_pal();
  auto_file_saveinfo_dat();
  auto_file_mapinfo_dat();
  FUN_8611_0ccf();
  FUN_78a8_0000();
  FUN_7ae8_0052();
  FUN_18a9_000e();
  return;
}


