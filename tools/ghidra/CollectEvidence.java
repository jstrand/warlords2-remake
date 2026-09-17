// Collect naming evidence for every function, for tools/autolabel.py.
// Args: <output tsv>
// Rows: <entry>\t<name>\ttext\t<table>\t<group>\t<index>   text lookups
//       <entry>\t<name>\tstr\t<dgroup offset>\t<string>      DGROUP strings
// Text lookups are calls to get_string / get_file_string / get_error_string
// (string_lookup tables 0/1/2: STRING.DAT, FILE.DAT, ERROR.DAT) whose group argument is a constant.
// @category War2
import java.io.File;
import java.io.PrintWriter;
import java.util.*;

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.*;
import ghidra.program.model.scalar.Scalar;
import ghidra.program.model.symbol.Reference;

public class CollectEvidence extends GhidraScript {
    static final int DGROUP = 0x4125;   // Ghidra segment
    static final String[] TABLES = {"get_string", "get_file_string", "get_error_string"};

    @Override
    public void run() throws Exception {
        Listing listing = currentProgram.getListing();
        try (PrintWriter out = new PrintWriter(new File(getScriptArgs()[0]))) {
            for (int t = 0; t < TABLES.length; t++) {
                List<Function> fs = getGlobalFunctions(TABLES[t]);
                if (fs.isEmpty()) {
                    printerr("missing " + TABLES[t]);
                    continue;
                }
                int calls = 0, noFunction = 0, notConstant = 0;
                for (Reference r : getReferencesTo(fs.get(0).getEntryPoint())) {
                    if (!r.getReferenceType().isCall())
                        continue;
                    calls++;
                    Function f = getFunctionContaining(r.getFromAddress());
                    if (f == null) {
                        noFunction++;
                        continue;
                    }
                    List<Long> args = pushedWords(listing.getInstructionAt(r.getFromAddress()));
                    if (args.isEmpty() || args.get(0) == null) {
                        notConstant++;
                        continue;
                    }
                    Long index = args.size() > 1 ? args.get(1) : null;
                    out.printf("%s\t%s\ttext\t%d\t%d\t%s%n", f.getEntryPoint(), f.getName(), t,
                            args.get(0), index == null ? "?" : String.valueOf((short) (long) index));
                }
                println(String.format("%s: %d calls, %d outside functions, %d non-constant",
                        TABLES[t], calls, noFunction, notConstant));
            }
            for (Function f : currentProgram.getFunctionManager().getFunctions(true)) {
                Instruction prev = null;
                for (Instruction ins : listing.getInstructions(f.getBody(), true)) {
                    Integer off = dgroupPush(prev, ins);
                    prev = ins;
                    if (off == null)
                        continue;
                    String s = readString(off);
                    if (s != null)
                        out.printf("%s\t%s\tstr\t%04x\t%s%n", f.getEntryPoint(), f.getName(), off,
                                s.replace("\t", " ").replace("\n", "\\n").replace("\r", "\\r"));
                }
            }
        }
        println("done");
    }

    // Constant 16-bit words pushed immediately before a call, first argument
    // first. A 32-bit push contributes two words (low, high). Non-constant
    // pushes are recorded as null and end the scan.
    private List<Long> pushedWords(Instruction call) {
        List<Long> words = new ArrayList<>();
        Instruction ins = call;
        while (words.size() < 2) {
            ins = ins.getPrevious();
            if (ins == null || !ins.getMnemonicString().equalsIgnoreCase("push"))
                break;
            Scalar s = ins.getScalar(0);
            if (s == null) {
                words.add(null);
                break;
            }
            long v = s.getUnsignedValue();
            if (isDword(ins)) {
                words.add(v & 0xffff);
                words.add((v >> 16) & 0xffff);
            }
            else {
                words.add(v & 0xffff);
            }
        }
        return words;
    }

    private Integer dgroupPush(Instruction prev, Instruction ins) {
        if (!ins.getMnemonicString().equalsIgnoreCase("push"))
            return null;
        Scalar s = ins.getScalar(0);
        if (s == null)
            return null;
        long v = s.getUnsignedValue();
        if (isDword(ins) && (v >> 16) == DGROUP)
            return (int) (v & 0xffff);
        if (!isDword(ins) && prev != null && prev.getMnemonicString().equalsIgnoreCase("push")
                && "DS".equalsIgnoreCase(prev.getDefaultOperandRepresentation(0)))
            return (int) (v & 0xffff);
        return null;
    }

    // 32-bit push in 16-bit code: operand-size prefix 66h.
    private boolean isDword(Instruction ins) {
        try {
            return ins.getByte(0) == 0x66;
        }
        catch (Exception e) {
            return false;
        }
    }

    private String readString(int off) {
        try {
            Address a = toAddr(String.format("%04x:%04x", DGROUP, off));
            StringBuilder b = new StringBuilder();
            for (int i = 0; i < 120; i++) {
                int c = currentProgram.getMemory().getByte(a.add(i)) & 0xff;
                if (c == 0)
                    return b.length() >= 3 ? b.toString() : null;
                if (c < 0x20 && c != '\n' && c != '\r' || c > 0x7e)
                    return null;
                b.append((char) c);
            }
        }
        catch (Exception e) {
            // unmapped
        }
        return null;
    }
}
