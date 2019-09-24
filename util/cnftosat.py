import sys

def isint(n):
    try:
        int(n)
        return True
    except ValueError:
        return False

def convert(ifile, ofile):
    satform = ""
    with open(ifile, 'r') as fp:
        for cnt, line in enumerate(fp):
            parts = line.split(" ")
            if isint(parts[0]) and int(parts[0]) != 0:
                if len(satform) > 0:
                    satform = satform + " & "
                satform = satform + "("
                if int(parts[0]) < 0:
                    satform = satform + "!"
                    parts[0] = parts[0][1:]
                satform = satform + "v" + parts[0]
                for part in parts[1:-1]:
                    satform = satform + " | "
                    if int(part) < 0:
                        satform = satform + "!"
                        part = part[1:]
                    satform = satform + "v" + part
                satform = satform + ")"

    with open(ofile, 'w') as wp:
        wp.write(satform)
        wp.flush()

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage : python " + sys.argv[0] + " <input_cnf_file> <output_sat_file>")
        sys.exit(1)
