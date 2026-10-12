
void __cdecl16far setup_garrisons(void)

{
  undefined1 uVar1;
  bool bVar2;
  int iVar3;
  undefined2 *puVar4;
  undefined2 in_DX;
  int iVar5;
  int city;
  undefined4 local_c;
  int local_6;
  
  iVar5 = city_count;
  while (city = iVar5 + -1, iVar5 != 0) {
    iVar5 = city;
    if (*(char *)(*(byte *)(*(int *)(city * 0x41 + 0x157f) * 0xe0 +
                           *(int *)(city * 0x41 + 0x157d) * 2) + 0x710) == '\n') {
      bVar2 = false;
      if (*(char *)(city * 0x41 + 0x1592) == '\x0f') {
        if (opt_neutral_cities == 0) {
          bVar2 = true;
        }
        else if ((0 < opt_neutral_cities) &&
                (local_6 = dice(1,4,opt_neutral_cities + -2), 3 < local_6)) {
          local_6 = 3;
        }
      }
      else {
        local_6 = 3;
      }
      if (!bVar2) {
        iVar3 = best_production_for(city,*(int *)(local_6 * 2 + 0xd60),-1,0);
        if (iVar3 == 0) {
          bVar2 = true;
        }
        else {
          iVar3 = FUN_7087_0000(city,(int)*(char *)(city * 0x41 + 0x15a9));
          uVar1 = *(undefined1 *)(city * 0x41 + iVar3 + 0x1593);
          in_DX = CONCAT11(0x3c,uVar1);
          *(undefined1 *)(city * 0x41 + 0x15a9) = uVar1;
          produce_garrison(city);
        }
      }
      if (bVar2) {
        puVar4 = (undefined2 *)FUN_1b62_0309();
        local_c = (undefined2 *)CONCAT22(in_DX,puVar4);
        *(undefined1 *)((int)puVar4 + 5) = 0xf;
        *(undefined1 *)(puVar4 + 2) = 0xb;
        *local_c = *(undefined2 *)(city * 0x41 + 0x157d);
        puVar4[1] = *(undefined2 *)(city * 0x41 + 0x157f);
        *(undefined1 *)(puVar4 + 4) = 1;
      }
    }
  }
  return;
}


