import re

#loop
epilogh = ""
while(epilogh != '4'):
    # epilogh xrhsth
    epilogh = input("Give your preference: (1: read new input file, 2: print statistics for a specific product,3: print statistics for a specific AFM, 4: exit the program)\n")

    if (epilogh == '1'):

        #diavasma arxeiou
        file_name = input("Give file\n")
        try:
            file = open(file_name, "r+", encoding='utf-8') #mono diavasma
        except IOError:
            print("exception") # na fugei
            epilogh = '0'  # h8ele kati to except

        if (epilogh == '1'):
            line = file.readline()
            while line:
                # an den einai olh h grammh apo "-" paei sthn epwmwnh / agnoei oles tis grammes mexri na brei paules 
                # if not re.fullmatch( "\-+\n" , line )
                if not((line == (len(line) - 1)  * '-' + '\n') and line != '\n'):
                    line = file.readline()
                    continue

                # epeksergasia apodikshs

                # expecting AFM
                line = file.readline()
                # elenxos format afm
                x = re.fullmatch(r"(ΑΦΜ|αφμ):[^\S\n]*[0-9]{10}[^\S\n]*\n", line)
                # apeteixe o elenxos
                if x == None:
                    if line != (len(line) - 1) * '-' + '\n':  # an den brei --- paei sthn epwmenh grammh
                        line = file.readline()
                    continue
                
                AFM_apodeikshs = int(''.join(filter(str.isdigit, line)))
                print(AFM_apodeikshs)

                # expecting proion
                line = file.readline()

                # prepei na iparxei toulaxiston 1 proion prin to sunolo
                if re.match("(συνολο|ΣΥΝΟΛΟ):", line) != None:
                   line = file.readline()
                   continue

                # arxikopoihsh counter
                ari8mos_prointwn = 0
                sunoliko_poso = 0

                pinakas_prointwn = []
                # prospelash proiontwn
                while(1):
                    # format pou 8eloume: proion: posothta timh sunolo \n
                    x = re.fullmatch(r"[α-ωΑ-Ωa-zA-Z0-9]+:[^\S\n]*[0-9]+[^\S\n]+[0-9]+(.[0-9]+)?[^\S\n]+[0-9]+(.[0-9]+)?[^\S\n]*\n" , line )
                    if x != None:
                        # apomonwsh ari8mwn
                        string_cut = line.split(":")
                        numbers = re.findall(r"\d*\.\d+|\d+", string_cut[1])

                        # posotita megalhterh tou mhden
                        calc_check = ( int(numbers[0]) > 0 ) and ( int(numbers[0]) * float(numbers[1]) == float(numbers[2]) )
                        # posotita*timh = sunolo
                        
                    # apetuxan oi elenxoi h den einai to format twn proiontwn
                    if(not calc_check or x == None):
                        # format sunolou
                        x = re.fullmatch( r"(συνολο|ΣΥΝΟΛΟ):[^\S\n]*[0-9]+(\.[0-9]+)?[^\S\n]*\n" , line )
                        if(x == None):
                            error_flag = 1
                        else:
                            error_flag = 0
                        break

                    #kseroume oti einai proion kai swsto
                    # ta kratame mexri na kseroume oti olh h apodeiksh einai swsth

                    pinakas_prointwn.append([])
                    pinakas_prointwn[ari8mos_prointwn].append(string_cut[0])
                    pinakas_prointwn[ari8mos_prointwn].append(numbers[0])
                    pinakas_prointwn[ari8mos_prointwn].append(numbers[1])
                    pinakas_prointwn[ari8mos_prointwn].append(numbers[2])

                    # upologismos seinolikou kostous
                    sunoliko_poso += numbers[2]
                    ari8mos_prointwn += 1

                    #epwmenh grammh
                    line = file.readline()
                # telos proiontwn


                # den hr8e sunolo
                if (error_flag):
                    continue

                # elenxos Σsunolwn = sunolo
                douleia sunolou

                line = epwmenh grammh
                elenxos(--------)
                    # ekleise h apodeiksh
                    apo8ikeush sta leksika
                #den ekleise
                else
                    line = epwmenh grammh
                    continue

            # teleiwnei otan erxete grammh null
                
            


        file.close()

    elif (epilogh == '2'):
        print('2')
    elif (epilogh == '3'):
        print('3')
        




#diavasma arxeiwn
#elenxos or8othtas
#upologismos kai apo8ikeush statistkwn

#ektupwsh statistikwn
