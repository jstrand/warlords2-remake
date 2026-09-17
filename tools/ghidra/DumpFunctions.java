// Dump functions in a range of Ghidra segments for identification.
// Args: <first seg hex> <last seg hex> <output dir>
// Writes <outdir>/index.txt (entry, size, call-ref count, callees, first
// bytes) and <outdir>/<entry>.c with the decompiled function.
// @category War2
import java.io.File;
import java.io.PrintWriter;
import java.util.*;

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.SegmentedAddress;
import ghidra.program.model.listing.Function;

public class DumpFunctions extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        int lo = Integer.parseInt(args[0], 16), hi = Integer.parseInt(args[1], 16);
        File out = new File(args[2]);
        out.mkdirs();
        DecompInterface dec = new DecompInterface();
        dec.openProgram(currentProgram);
        try (PrintWriter idx = new PrintWriter(new File(out, "index.txt"))) {
            for (Function f : currentProgram.getFunctionManager().getFunctions(true)) {
                if (!(f.getEntryPoint() instanceof SegmentedAddress a))
                    continue;
                int seg = a.getSegment();
                if (seg < lo || seg > hi)
                    continue;
                int refs = 0;
                for (var r : getReferencesTo(f.getEntryPoint()))
                    if (r.getReferenceType().isCall())
                        refs++;
                Set<String> callees = new TreeSet<>();
                for (Function c : f.getCalledFunctions(monitor))
                    callees.add(c.getName());
                byte[] b = new byte[12];
                currentProgram.getMemory().getBytes(f.getEntryPoint(), b);
                StringBuilder hex = new StringBuilder();
                for (byte x : b)
                    hex.append(String.format("%02x", x & 0xff));
                idx.printf("%s %-24s size=%-5d calls=%-4d bytes=%s callees=%s%n",
                        f.getEntryPoint(), f.getName(), f.getBody().getNumAddresses(),
                        refs, hex, callees);
                var res = dec.decompileFunction(f, 60, monitor).getDecompiledFunction();
                try (PrintWriter c = new PrintWriter(new File(out,
                        f.getEntryPoint().toString().replace(':', '_') + ".c"))) {
                    c.println(res == null ? "// decompile failed" : res.getC());
                }
            }
        }
        println("done");
    }
}
