
void __cdecl16far sage_build_offer_list(void)

{
  bool bVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  char *src;
  
  iVar4 = 0;
  bVar1 = false;
  bVar2 = false;
  iVar5 = 0;
  do {
    if (site_count <= iVar5) {
      DAT_451b_08ce = iVar4;
      return;
    }
    if (((*(int *)(iVar5 * 0x1f + 0x82c) != 0) &&
        ((*(uint *)(iVar5 * 0x1f + 0x82e) & 1 << ((byte)current_player & 0x1f)) == 0)) &&
       (iVar3 = map_distance(*(int *)(iVar5 * 0x1f + 0x811),*(int *)(iVar5 * 0x1f + 0x813),
                             DAT_451b_01ca,DAT_451b_01c8), iVar3 < 0x23)) {
      if (*(char *)(iVar5 * 0x1f + 0x829) == '\x04') {
        if (!bVar1) {
          bVar1 = true;
          ((undefined2 *)&DAT_451b_0c9c)[iVar4] = 0xffff;
          src = (char *)0x41250c84;
          goto LAB_6536_1730;
        }
      }
      else if (*(char *)(iVar5 * 0x1f + 0x829) == '\x05') {
        if (!bVar2) {
          bVar2 = true;
          ((undefined2 *)&DAT_451b_0c9c)[iVar4] = 0xfffe;
          src = s_Allies_4125_0c89;
LAB_6536_1730:
          strcpy((char *)CONCAT22(0x4125,(undefined1 *)&DAT_451b_08dc + iVar4 * 0x20),src);
          iVar4 = iVar4 + 1;
        }
      }
      else if (*(char *)(iVar5 * 0x1f + 0x829) == '\x02') {
        ((undefined2 *)&DAT_451b_0c9c)[iVar4] = (int)*(char *)(iVar5 * 0x1f + 0x82a);
        src = (char *)CONCAT22(0x3c04,(char *)(((undefined2 *)&DAT_451b_0c9c)[iVar4] * 0x1d + 0xce9)
                              );
        goto LAB_6536_1730;
      }
    }
    iVar5 = iVar5 + 1;
  } while( true );
}


