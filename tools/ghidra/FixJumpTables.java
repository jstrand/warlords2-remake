// Repair CS-relative jump tables (`jmp word cs:[bx+table]`).
//
// Ghidra evaluates CS from the instruction's normalised address (e.g. 4000:xxxx
// instead of the real 484e:xxxx), reads the table from the wrong place and
// invents garbage targets, which breaks decompilation of every function with a
// switch. This reads each table from the function's real segment, sized by the
// preceding `cmp bx, n` bounds check, replaces the computed-jump references,
// stores a switch override for the decompiler and fixes the function body.
// Run after auto-analysis (SetupWar2 runs first as the pre-script).
// @category War2
import java.util.ArrayList;

import ghidra.app.cmd.function.CreateFunctionCmd;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.SegmentedAddress;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.pcode.JumpTable;
import ghidra.program.model.symbol.RefType;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.SourceType;

public class FixJumpTables extends GhidraScript {
    @Override
    public void run() throws Exception {
        Memory mem = currentProgram.getMemory();
        int fixed = 0, skipped = 0;
        for (Instruction ins : currentProgram.getListing().getInstructions(true)) {
            byte[] b;
            try {
                b = ins.getBytes();
            }
            catch (Exception e) {
                continue;
            }
            // 2E FF A7 lo hi : jmp word cs:[bx+disp16]
            if (b.length != 5 || (b[0] & 0xff) != 0x2e || (b[1] & 0xff) != 0xff || (b[2] & 0xff) != 0xa7)
                continue;
            Function f = getFunctionContaining(ins.getAddress());
            if (f == null || !(f.getEntryPoint() instanceof SegmentedAddress entry)) {
                skipped++;
                continue;
            }
            int seg = entry.getSegment();
            int table = (b[3] & 0xff) | (b[4] & 0xff) << 8;
            int count = boundsCount(ins);
            if (count <= 0) {
                skipped++;
                continue;
            }
            ArrayList<Address> dests = new ArrayList<>();
            for (int k = 0; k < count; k++) {
                Address slot = toAddr(String.format("%04x:%04x", seg, table + 2 * k));
                int off = mem.getShort(slot) & 0xffff;
                dests.add(toAddr(String.format("%04x:%04x", seg, off)));
            }
            for (Reference r : ins.getReferencesFrom())
                if (r.getReferenceType().isComputed())
                    currentProgram.getReferenceManager().delete(r);
            for (Address d : dests) {
                disassemble(d);
                ins.addOperandReference(0, d, RefType.COMPUTED_JUMP, SourceType.USER_DEFINED);
            }
            new JumpTable(ins.getAddress(), dests, true, 0).writeOverride(f);
            CreateFunctionCmd.fixupFunctionBody(currentProgram, f, monitor);
            fixed++;
        }
        println("jump tables fixed: " + fixed + ", skipped: " + skipped);
    }

    // Look back a few instructions for `cmp bx, n` (bounds check before the
    // index is doubled); the table then has n + 1 entries.
    private int boundsCount(Instruction jmp) {
        Instruction ins = jmp;
        for (int i = 0; i < 6; i++) {
            ins = ins.getPrevious();
            if (ins == null)
                return -1;
            if (ins.getMnemonicString().equalsIgnoreCase("cmp")
                    && "BX".equalsIgnoreCase(ins.getDefaultOperandRepresentation(0))
                    && ins.getScalar(1) != null) {
                long n = ins.getScalar(1).getUnsignedValue();
                return n < 64 ? (int) n + 1 : -1;
            }
        }
        return -1;
    }
}
