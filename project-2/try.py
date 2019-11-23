import re

line = "ΠΡΟΙΟΝ7: 10     13.5  1345   \n"

# for i in range(500000):
x = re.fullmatch(
    r"[α-ωΑ-Ωa-zA-Z0-9]+:[^\S\n]*[0-9]+[^\S\n]+[0-9]+(\.[0-9]+)?[^\S\n]+[0-9]+(\.[0-9]+)?[^\S\n]*\n", line)

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
pinakas_prointwn.append([])
pinakas_prointwn.append([])

pinakas_prointwn[ari8mos_prointwn].append(string_cut[0])
pinakas_prointwn[ari8mos_prointwn].append(numbers[0])
pinakas_prointwn[ari8mos_prointwn].append(numbers[1])
pinakas_prointwn[ari8mos_prointwn].append(numbers[2])

pinakas_prointwn[1].append("sccd")
pinakas_prointwn[1].append(12)
pinakas_prointwn[1].append(32)
pinakas_prointwn[1].append(11)

pinakas_prointwn[2].append("sccd")
pinakas_prointwn[2].append(12)
pinakas_prointwn[2].append(32)
pinakas_prointwn[2].append(11)

#print(pinakas_prointwn)
pinakas_prointwn[3].append(string_cut[0])
pinakas_prointwn[3].append(12)
pinakas_prointwn[3].append(32)
pinakas_prointwn[3].append(11)

afm = [ "1234567890" , "1234563890" , "1234567890" , "1234567890" ]

proionta = {}
afms = {}


for i in range(4):

    # prointa
    if proionta.get( pinakas_prointwn[i][0] ) != None:
        # uparxei proion

        value = proionta[ pinakas_prointwn[i][0] ].get( afm[i] )

        if value != None:
            # uparxei afm
            proionta[ pinakas_prointwn[i][0] ].update( {afm[i] : pinakas_prointwn[i][2] + float(value) } )
        else:
            # den uparxei afm
            proionta[ pinakas_prointwn[i][0] ].update( {afm[i] : pinakas_prointwn[i][2]} )

    else:
        # den uparxei proion
        proionta[ pinakas_prointwn[i][0] ] = { afm[i] : pinakas_prointwn[i][2] }

    # afms
    if afms.get( afm[i] ) != None:
        # uparxei afm

        value = afms[ afm[i] ].get( pinakas_prointwn[i][0] )

        if value != None:
            # uparxei proion
            afms[ afm[i] ].update( { pinakas_prointwn[i][0] : pinakas_prointwn[i][1] + int(value) } )
        else:
            # den yparxei proion
            afms[ afm[i] ].update( { pinakas_prointwn[i][0] : pinakas_prointwn[i][1] } )
    
    else:
        # den yparxei afm
        afms[ afm[i] ] = { pinakas_prointwn[i][0] : pinakas_prointwn[i][1] }


print(proionta)

print("\n", afms)