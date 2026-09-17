// Dump the disassembly of every function in a range of Ghidra segments.
// Args: <first seg hex> <last seg hex> <output file> [max instructions]
// @category War2
import java.io.File;
import java.io.PrintWriter;

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.SegmentedAddress;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;

public class DumpDisasm extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        int lo = Integer.parseInt(args[0], 16), hi = Integer.parseInt(args[1], 16);
        int max = args.length > 3 ? Integer.parseInt(args[3]) : 80;
        try (PrintWriter out = new PrintWriter(new File(args[2]))) {
            for (Function f : currentProgram.getFunctionManager().getFunctions(true)) {
                if (!(f.getEntryPoint() instanceof SegmentedAddress a)
                        || a.getSegment() < lo || a.getSegment() > hi)
                    continue;
                out.printf("#### %s %s%n", f.getEntryPoint(), f.getName());
                int n = 0;
                for (Instruction ins : currentProgram.getListing().getInstructions(f.getBody(), true)) {
                    if (n++ == max) {
                        out.println("   ...");
                        break;
                    }
                    StringBuilder ops = new StringBuilder();
                    for (int i = 0; i < ins.getNumOperands(); i++)
                        ops.append(i == 0 ? " " : ",").append(ins.getDefaultOperandRepresentation(i));
                    out.printf("%04x %s%s%n", ((SegmentedAddress) ins.getAddress()).getSegmentOffset(),
                            ins.getMnemonicString(), ops);
                }
            }
        }
        println("done");
    }
}
