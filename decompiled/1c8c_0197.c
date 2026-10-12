
void __cdecl16far auto_str_move_mouse_to_drag(char param_1)

{
  undefined2 in_DX;
  
  if (param_1 == '5') {
    FUN_8611_0565();
  }
  else {
    if ((int)param_1 - 0x31U < 9) {
                    /* WARNING: Switch is manually overridden */
      switch((undefined4)*(undefined2 *)(((int)param_1 - 0x31U) * 2 + 0x1eb)) {
      case 0x1ca7d:
        in_DX = 0;
        break;
      case 0x1ca81:
        in_DX = 1;
        break;
      case 0x1ca86:
        in_DX = 2;
        break;
      case 0x1ca8b:
        in_DX = 3;
        break;
      case 0x1ca90:
        in_DX = 4;
        break;
      case 0x1ca95:
        in_DX = 5;
        break;
      case 0x1ca9a:
        in_DX = 6;
        break;
      case 0x1ca9f:
        in_DX = 7;
      }
    }
    FUN_1c8c_041f(in_DX);
  }
  return;
}


