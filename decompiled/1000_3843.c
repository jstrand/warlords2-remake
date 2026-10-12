
undefined2 __cdecl16far
int86x(byte param_1,undefined4 param_2,undefined2 *param_3,undefined2 *param_4)

{
  uint *puVar1;
  uint uVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  char cVar5;
  undefined2 uVar6;
  undefined2 uVar7;
  undefined2 uVar8;
  int iVar9;
  undefined2 *puVar10;
  undefined2 uVar11;
  undefined2 uVar12;
  undefined2 uVar13;
  byte bVar14;
  byte bVar15;
  byte in_AF;
  byte bVar16;
  byte bVar17;
  byte in_TF;
  byte in_IF;
  byte bVar18;
  byte in_NT;
  undefined4 uVar19;
  undefined1 local_10;
  undefined1 local_f;
  byte local_e;
  undefined3 local_d;
  code *local_a;
  undefined2 local_8;
  code *local_6;
  
  local_6 = (code *)&local_10;
  local_10 = (code)0x55;
  local_f = 0xcd;
  local_e = param_1;
  local_d = CONCAT12(local_d._2_1_,0xcb5d);
  bVar14 = param_1 < 0x25;
  bVar18 = SBORROW1(param_1,'%');
  cVar5 = param_1 - 0x25;
  bVar17 = cVar5 < '\0';
  bVar16 = cVar5 == '\0';
  bVar15 = (POPCOUNT(cVar5) & 1U) == 0;
  if (!(bool)bVar14) {
    bVar14 = param_1 < 0x26;
    bVar18 = SBORROW1(param_1,'&');
    cVar5 = param_1 - 0x26;
    bVar17 = cVar5 < '\0';
    bVar16 = cVar5 == '\0';
    bVar15 = (POPCOUNT(cVar5) & 1U) == 0;
    if ((bool)bVar14 || (bool)bVar16) {
      local_d = 0x68f36;
      local_8 = 0xcb5d;
      local_a = local_6;
    }
  }
  uVar11 = (undefined2)((ulong)param_4 >> 0x10);
  uVar13 = *param_4;
  uVar3 = ((undefined2 *)param_4)[3];
  uVar12 = (undefined2)((ulong)param_2 >> 0x10);
  iVar9 = (int)param_2;
  uVar8 = *(undefined2 *)(iVar9 + 2);
  uVar7 = *(undefined2 *)(iVar9 + 4);
  uVar4 = *(undefined2 *)(iVar9 + 10);
  uVar12 = *(undefined2 *)(iVar9 + 8);
  uVar19 = (*local_6)(0x1000,0x4125);
  uVar6 = (undefined2)uVar19;
  *param_4 = uVar13;
  ((undefined2 *)param_4)[3] = uVar3;
  uVar13 = (undefined2)((ulong)param_3 >> 0x10);
  puVar10 = (undefined2 *)param_3;
  puVar10[4] = uVar12;
  puVar10[7] = (uint)(in_NT & 1) * 0x4000 | (uint)(bVar18 & 1) * 0x800 | (uint)(in_IF & 1) * 0x200 |
               (uint)(in_TF & 1) * 0x100 | (uint)(bVar17 & 1) * 0x80 | (uint)(bVar16 & 1) * 0x40 |
               (uint)(in_AF & 1) * 0x10 | (uint)(bVar15 & 1) * 4 | (uint)(bVar14 & 1);
  puVar10[6] = (uint)(in_NT & 1) * 0x4000 | (uint)(bVar18 & 1) * 0x800 | (uint)(in_IF & 1) * 0x200 |
               (uint)(in_TF & 1) * 0x100 | (uint)(bVar17 & 1) * 0x80 | (uint)(bVar16 & 1) * 0x40 |
               (uint)(in_AF & 1) * 0x10 | (uint)(bVar15 & 1) * 4 | (uint)(bVar14 & 1);
  puVar1 = puVar10 + 6;
  *puVar1 = *puVar1 & 1;
  uVar2 = *puVar1;
  puVar10[5] = uVar4;
  puVar10[3] = (int)((ulong)uVar19 >> 0x10);
  puVar10[2] = uVar7;
  puVar10[1] = uVar8;
  *param_3 = uVar6;
  if (uVar2 != 0) {
    __IOerror(uVar6);
  }
  return uVar6;
}


