// Dump every function that calls a named function, for triage.
// Args: <function name> <output dir>   (defaults: dice build/ghidra/callers)
// Writes <outdir>/index.txt (one block per caller: call sites with the
// constant arguments pushed before each call, strings referenced, and who
// calls the caller) and <outdir>/<entry>.c with the decompiled function.
// @category War2
import java.io.File;
import java.io.PrintWriter;
import java.util.*;

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.*;
import ghidra.program.model.scalar.Scalar;
import ghidra.program.model.symbol.Reference;

public class DumpCallers extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        String name = args.length > 0 ? args[0] : "dice";
        File out = new File(args.length > 1 ? args[1] : "build/ghidra/callers");
        out.mkdirs();

        Function target = getGlobalFunctions(name).get(0);
        Map<Function, List<Address>> sites = new TreeMap<>(
                Comparator.comparing(Function::getEntryPoint));
        for (Reference r : getReferencesTo(target.getEntryPoint())) {
            if (!r.getReferenceType().isCall())
                continue;
            Function f = getFunctionContaining(r.getFromAddress());
            if (f != null)
                sites.computeIfAbsent(f, k -> new ArrayList<>()).add(r.getFromAddress());
        }

        DecompInterface dec = new DecompInterface();
        dec.openProgram(currentProgram);
        Listing listing = currentProgram.getListing();
        try (PrintWriter idx = new PrintWriter(new File(out, "index.txt"))) {
            for (var e : sites.entrySet()) {
                Function f = e.getKey();
                idx.printf("== %s %s size=%d%n", f.getEntryPoint(), f.getName(),
                        f.getBody().getNumAddresses());
                for (Address call : e.getValue())
                    idx.printf("   call %s  pushes: %s%n", call, pushes(listing, call));
                Set<String> strings = new TreeSet<>();
                for (Instruction ins : listing.getInstructions(f.getBody(), true))
                    for (Reference r : ins.getReferencesFrom()) {
                        Data d = listing.getDataAt(r.getToAddress());
                        if (d != null && d.hasStringValue())
                            strings.add(String.valueOf(d.getValue()));
                    }
                if (!strings.isEmpty())
                    idx.printf("   strings: %s%n", strings);
                Set<String> callers = new TreeSet<>();
                for (Function c : f.getCallingFunctions(monitor))
                    callers.add(c.getEntryPoint() + " " + c.getName());
                idx.printf("   called by: %s%n", callers);

                DecompileResults res = dec.decompileFunction(f, 120, monitor);
                try (PrintWriter c = new PrintWriter(new File(out,
                        f.getEntryPoint().toString().replace(':', '_') + ".c"))) {
                    c.println(res.getDecompiledFunction() != null
                            ? res.getDecompiledFunction().getC() : "// decompile failed");
                }
            }
        }
        println("dumped " + sites.size() + " callers of " + name + " to " + out);
    }

    // Immediate pushes preceding the call, nearest first (arg1, arg2, ...).
    // A 32-bit "push dword imm" is split into its low and high words.
    private String pushes(Listing listing, Address call) {
        List<String> vals = new ArrayList<>();
        Instruction ins = listing.getInstructionAt(call);
        for (int i = 0; i < 4 && vals.size() < 3; i++) {
            ins = ins == null ? null : ins.getPrevious();
            if (ins == null || !ins.getMnemonicString().equalsIgnoreCase("push"))
                break;
            Scalar s = ins.getScalar(0);
            if (s == null) {
                vals.add(ins.getDefaultOperandRepresentation(0));
            }
            else if (ins.getLength() == 6) {
                long v = s.getUnsignedValue();
                vals.add(String.valueOf((short) (v & 0xffff)));
                vals.add(String.valueOf((short) (v >> 16)));
            }
            else {
                vals.add(String.valueOf(s.getSignedValue()));
            }
        }
        return String.join(", ", vals);
    }
}
