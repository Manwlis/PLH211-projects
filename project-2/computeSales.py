import re
import math


# ta regex me paren8eseis isws 8eloun ?:

proionta = {}
afm = {}

#loop
epilogh = ""
while(epilogh != '4'):
    # epilogh xrhsth
    epilogh = input("\nGive your preference: (1: read new input file, 2: print statistics for a specific product,3: print statistics for a specific AFM, 4: exit the program)\n")

    if (epilogh == '1'):

        #diavasma arxeiou
        file_name = input("Give file\n")
        try:
            # anoigma arxeiou
            file = open(file_name, "r+", encoding='utf-8') #mono diavasma

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
                
                afm_apodeikshs = ''.join(filter(str.isdigit, line))

                # expecting proion
                line = file.readline()


                # arxikopoihsh counter
                ari8mos_prointwn = 0
                sunoliko_poso = 0

                pinakas_prointwn = []
                # prospelash proiontwn
                while(1):
                    # format pou 8eloume: proion: posothta timh sunolo \n
                    x = re.fullmatch(r"[^:\n]+:[^\S\n]*[0-9]+[^\S\n]+[0-9]+(\.[0-9]+)?[^\S\n]+[0-9]+(\.[0-9]+)?[^\S\n]*\n" , line )

                    calc_check = False

                    if x != None:
                        # apomonwsh ari8mwn
                        string_cut = line.split(":")
                        numbers = re.findall(r"\d+\.\d+|\d+", string_cut[1])

                        # posotita megalhterh tou mhden kai posotita*timh = sunolo
                        calc_check = ( int(numbers[0]) > 0 ) and math.isclose( int(numbers[0]) * float(numbers[1]) , float(numbers[2]) , abs_tol=0.001)

                   
                    # apetuxan oi elenxoi h den einai to format twn proiontwn
                    if( calc_check == False):
                        # format sunolou
                        x = re.fullmatch( r"(συνολο|ΣΥΝΟΛΟ):[^\S\n]*[0-9]+(\.[0-9]+)?[^\S\n]*\n" , line )

                        if( x == None or ari8mos_prointwn == 0 ):
                            error_flag = 1
                        else:
                            error_flag = 0
                        break

                    #kseroume oti einai proion kai swsto
                    # ta kratame mexri na kseroume oti olh h apodeiksh einai swsth

                    pinakas_prointwn.append([])
                    pinakas_prointwn[ari8mos_prointwn].append( string_cut[0].capitalize() )
                    pinakas_prointwn[ari8mos_prointwn].append( float(numbers[2]) )

                    # upologismos sunolikou kostous
                    sunoliko_poso += pinakas_prointwn[ari8mos_prointwn][1]
                    ari8mos_prointwn += 1

                    #epwmenh grammh
                    line = file.readline()
                # telos proiontwn

                # den hr8e sunolo
                if (error_flag):
                    continue

                # extract sunolo apo string
                teliko_sunolo = float(re.findall( r"[0-9]+(?:\.[0-9]+)?" , line )[0])

                line = file.readline()
                # elenxos Σsunolwn = sunolo
                if math.isclose( teliko_sunolo , sunoliko_poso , abs_tol=0.001 ) == False:
                    continue


                if re.fullmatch( r"\-+\n?" , line ) != None:
                    # h apodeiksh ekleise swsta

                    # apo8ikeush me th seira pou diavasthkan
                    for i in range(ari8mos_prointwn):

                        # prointa
                        if proionta.get( pinakas_prointwn[i][0] ) != None:
                            # uparxei proion

                            value = proionta[ pinakas_prointwn[i][0] ].get( afm_apodeikshs )

                            if value != None:
                                # uparxei afm
                                proionta[ pinakas_prointwn[i][0] ].update( {afm_apodeikshs : pinakas_prointwn[i][1] + float(value)} )
                            else:
                                # den uparxei afm
                                proionta[ pinakas_prointwn[i][0] ].update( {afm_apodeikshs : pinakas_prointwn[i][1]} )

                        else:
                            # den uparxei proion
                            proionta[ pinakas_prointwn[i][0] ] = { afm_apodeikshs : pinakas_prointwn[i][1] }

                        # afms
                        if afm.get( afm_apodeikshs ) != None:
                            # uparxei afm

                            value = afm[ afm_apodeikshs ].get( pinakas_prointwn[i][0] )

                            if value != None:
                                # uparxei proion
                                afm[ afm_apodeikshs ].update( { pinakas_prointwn[i][0] : pinakas_prointwn[i][1] + float(value) } )
                            else:
                                # den yparxei proion
                                afm[ afm_apodeikshs ].update( { pinakas_prointwn[i][0] : pinakas_prointwn[i][1] } )
                        
                        else:
                            # den yparxei afm
                            afm[ afm_apodeikshs ] = { pinakas_prointwn[i][0] : pinakas_prointwn[i][1] }
                else:
                    # h apodeiksh den ekleise swsta
                    line = file.readline()
                    continue

            # teleiwnei otan erxete grammh null
            
            # kleisiomo arxeiou
            file.close()
        except IOError:
            epilogh = '0'  # h8ele kati to except


    elif (epilogh == '2'):
        
        proion_to_print = input("Give product name\n").capitalize()

        # pame sto eswteriko leksiko
        x = proionta.get( proion_to_print )
       
        try:
            for key in sorted(x.keys()):
                print(key, "%.2f" % ( x[key] ))
               
        except AttributeError:
            epilogh = 0


    elif (epilogh == '3'):
        
        afm_to_print = input("Give AFM\n").capitalize()
        
        x = afm.get( afm_to_print )

        try:
            for key in sorted(x.keys()):
                print(key, "%.2f" % ( x[key] ))
                
        except AttributeError:
            epilogh = 0
