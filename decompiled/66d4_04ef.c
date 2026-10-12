
void __cdecl16far load_item_pool(int reserved_count)

{
  uint uVar1;
  int iVar2;
  uint unaff_DI;
  int iVar3;
  void *unaff_SS;
  char local_632 [19];
  char local_61f [1431];
  int local_88 [22];
  char local_5c [2];
  undefined1 local_5a;
  int local_c;
  int local_a;
  int local_8;
  char local_5;
  int local_4;
  
  sprintf((char *)CONCAT22(unaff_SS,local_5c),(char *)0x41250caf,0x5da0,0x4125,0x5d78,0x4125);
  local_4 = open((char *)CONCAT22(unaff_SS,local_5c),2);
  if (-1 < local_4) {
    read(local_4,(void *)CONCAT22(2,unaff_SS),unaff_DI);
    local_5a = 0;
    sscanf((char *)CONCAT22(unaff_SS,local_5c),(char *)0x41250cb9,&local_8);
    read(local_4,(void *)CONCAT22(2,unaff_SS),unaff_DI);
    for (iVar2 = 0; iVar2 < local_8; iVar2 = iVar2 + 1) {
      read(local_4,(void *)CONCAT22(0x14,unaff_SS),unaff_DI);
      local_61f[iVar2 * 0x1d] = '\0';
      read(local_4,(void *)CONCAT22(1,unaff_SS),unaff_DI);
      read(local_4,(void *)CONCAT22(1,unaff_SS),unaff_DI);
      local_61f[iVar2 * 0x1d + 1] = local_5 + -0x30;
      read(local_4,(void *)CONCAT22(1,unaff_SS),unaff_DI);
      read(local_4,(void *)CONCAT22(1,unaff_SS),unaff_DI);
      local_61f[iVar2 * 0x1d + 2] = local_5 + -0x30;
      read(local_4,(void *)CONCAT22(2,unaff_SS),unaff_DI);
      for (iVar3 = 0; uVar1 = strlen((char *)CONCAT22(unaff_SS,local_632 + iVar2 * 0x1d)),
          iVar3 < (int)uVar1; iVar3 = iVar3 + 1) {
        if (local_632[iVar2 * 0x1d + iVar3] == ' ') {
          local_632[iVar2 * 0x1d + iVar3] = '\0';
        }
        if (local_632[iVar2 * 0x1d + iVar3] == '_') {
          local_632[iVar2 * 0x1d + iVar3] = ' ';
        }
      }
    }
    close(local_4);
    for (iVar2 = 0; iVar2 < 8; iVar2 = iVar2 + 1) {
      local_88[iVar2] = -1;
    }
    iVar2 = 8;
LAB_66d4_07c6:
    if (iVar2 < 0x16) {
LAB_66d4_074c:
      local_c = 0;
      do {
        iVar3 = local_a;
        if (local_c != 0) {
          local_88[iVar2] = local_a;
          strcpy((char *)CONCAT22(0x3c04,(char *)(iVar2 * 0x1d + 0xce9)),
                 (char *)CONCAT22(unaff_SS,local_632 + iVar3 * 0x1d));
          *(char *)(iVar2 * 0x1d + 0xcfd) = local_61f[local_a * 0x1d + 1];
          *(char *)(iVar2 * 0x1d + 0xcfe) = local_61f[local_a * 0x1d + 2];
          iVar2 = iVar2 + 1;
          goto LAB_66d4_07c6;
        }
        local_a = dice(1,local_8,-1);
        local_c = 1;
        for (iVar3 = 0; iVar3 < iVar2; iVar3 = iVar3 + 1) {
          if (local_88[iVar3] == local_a) {
            local_c = 0;
          }
        }
        if (local_c == 0) {
LAB_66d4_071a:
          if (local_c == 0) goto LAB_66d4_074c;
          __F_SPUSH();
          iVar3 = item_reserved();
          if ((iVar3 != 0) || (iVar2 < reserved_count + 8)) goto LAB_66d4_074c;
        }
        else {
          __F_SPUSH();
          iVar3 = item_reserved();
          if ((iVar3 == 0) || (reserved_count + 8 <= iVar2)) goto LAB_66d4_071a;
        }
        local_c = 1;
      } while( true );
    }
  }
  return;
}


