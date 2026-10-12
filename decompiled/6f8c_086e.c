
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_6f8c_086e(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 unaff_SS;
  char *pcVar5;
  char local_56 [82];
  undefined1 local_4 [2];
  
  if (param_1 != 0) {
    FUN_54f6_07e4(0,0x6e,0x30,0x34);
  }
  FUN_78a8_06ae(2,*(undefined2 *)(current_player * 2 + 0xa0),
                *(undefined2 *)(current_player * 2 + 0xb0),3);
  FUN_6f8c_076d();
  iVar2 = 0;
  do {
    if (_LAB_4125_0e51_1 <= iVar2) {
      FUN_6f8c_07d3();
      return;
    }
    iVar3 = DAT_451b_0502 + iVar2;
    FUN_78a8_06ae(2,0xf,0,3);
    sprintf((char *)CONCAT22(unaff_SS,local_56),(char *)0x41250e64,iVar3 + 1);
    FUN_7ecb_00bf(0x138,iVar2 * 0x1e + 0xb0,local_56);
    FUN_8611_08be((int)*(char *)((int)DAT_451b_0504 + iVar3 + 100),current_player,0x342a,0x4125,
                  0x148,iVar2 * 0x1e + 0xaa,0,0,0,0,1,0,0);
    uVar4 = (undefined2)((ulong)DAT_451b_0504 >> 0x10);
    iVar1 = (int)DAT_451b_0504;
    if (*(char *)(iVar1 + iVar3) == '\0') {
      iVar1 = (int)*(char *)(iVar1 + iVar3 + 300);
      if (iVar1 < 100) {
        iVar1 = iVar1 * 0x41 + 0x1581;
        uVar4 = 0x3c04;
        pcVar5 = (char *)0xe77;
      }
      else {
        FUN_7563_0b47(current_player,local_4);
        uVar4 = 0x4125;
        iVar1 = 0xe6e;
        pcVar5 = (char *)0xe67;
      }
LAB_6f8c_0998:
      sprintf((char *)CONCAT22(unaff_SS,local_56),(char *)CONCAT22(0x4125,pcVar5),iVar1,uVar4);
    }
    else {
      if (*(char *)(iVar1 + iVar3) == '\x01') {
        iVar1 = *(char *)(iVar1 + iVar3 + 200) * 0x41 + 0x1581;
        uVar4 = 0x3c04;
        pcVar5 = (char *)&LAB_4125_0e7e;
        goto LAB_6f8c_0998;
      }
      strcpy((char *)CONCAT22(unaff_SS,local_56),
             (char *)CONCAT22(0x3c04,(char *)(*(char *)(iVar1 + iVar3 + 200) * 0x41 + 0x1581)));
    }
    FUN_7ecb_00bf(0x170,iVar2 * 0x1e + 0xb0,local_56);
    iVar2 = iVar2 + 1;
  } while( true );
}


