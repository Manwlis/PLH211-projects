import re
line = "ΠΡΟΙΟΝ7: 10     13.5  1345   \n"

#for i in range(500000):
x = re.fullmatch(r"[α-ωΑ-Ωa-zA-Z0-9]+:[^\S\n]*[0-9]+[^\S\n]+[0-9]+(\.[0-9]+)?[^\S\n]+[0-9]+(\.[0-9]+)?[^\S\n]*\n", line)

aaa = line.split(":")[1]

numbers = re.findall(r"\d*\.\d+|\d+", aaa)
# # apeteixe o elenxos
# print(x)
# print(numbers)
# print(numbers[0])


string_cut = line.split(":")

ari8mos_prointwn = 0

pinakas_prointwn = []
pinakas_prointwn.append([])
pinakas_prointwn.append([])

pinakas_prointwn[ari8mos_prointwn].append(string_cut[0])
pinakas_prointwn[ari8mos_prointwn].append(numbers[0])
pinakas_prointwn[ari8mos_prointwn].append(numbers[1])
pinakas_prointwn[ari8mos_prointwn].append(numbers[2])



print(pinakas_prointwn)

pinakas_prointwn = []

print(pinakas_prointwn)
