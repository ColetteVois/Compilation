#include "ast.h"
#include "symboles.h"

// Cette variable globale vaut au depart 0, et est incrementee a chaque fois
int countDigraph;

// Cette variable permet de declarer des etiquettes a la volee
int currentLabel=0;
int crt=0;
int nbpf=0;

nodeType *createNumericNode(float v)
{
	nodeType *p;

	if ((p=(nodeType*)malloc(sizeof(nodeType))) == NULL)
	{
		printf("out of memory error\n");
		exit(1);
	}

	p->type=typeNumeric;
	p->t_numeric.valeur=v;
	
	return p;
}

nodeType *createOperatorNode(int oper, int nops, ...) 
{
    	va_list ap;
    	nodeType *p;
    	int i;

    	/* allocate node */
    	if ((p = (nodeType*)malloc(sizeof(nodeType))) == NULL)
	{
		printf("out of memory error\n");
		exit(1);
	}
    	if ((p->t_oper.op = (nodeType**)malloc(nops * sizeof(nodeType))) == NULL)
	{
		printf("out of memory error\n");
		exit(1);
	}

    	/* copy information */
    	p->type = typeOperator;
    	p->t_oper.oper = oper;
    	p->t_oper.nOperands = nops;
    	va_start(ap, nops);
    	for (i = 0; i < nops; i++)
        	p->t_oper.op[i] = va_arg(ap, nodeType*);
    	va_end(ap);
    	return p;
}

nodeType *createIdentifierNode(char *id, int funcNum, int index)
{
        nodeType *p;

        if ((p=malloc(sizeof(nodeType))) == NULL)
        {
                printf("out of memory error\n");
                exit(1);
        }

        p->type=typeIdentifier;
        p->t_identifier.ident=strdup(id);
        p->t_identifier.funcNum=funcNum;
        p->t_identifier.index=index;

        return p;
}

void generateAsmRec(nodeType *n, FILE *fout)
{
    int label1,label2;	

	if (n==NULL)
		return;
 
    switch (n->type)
        {
                case typeNumeric:
                        {
                            printf("noeud numérique main principale %f\n",n->t_numeric.valeur);
                            fprintf(fout,"\tpushr\t%3.f\n",n->t_numeric.valeur);
                        }
                        break;
                case typeIdentifier:
                        {
               
                            fprintf(fout,"\tpush\t%d\n",n->t_identifier.index);
                            //fprintf(fout,"\tmts\t%d\n");

                        }
                        break;
                case typeOperator:
                        {
                                switch (n->t_oper.oper)
                                {
                                        case OPER_APPEL_FONCTION:
                                                //generateAsmRec(n->t_oper.op[0],fout);
                                                generateAsmRec(n->t_oper.op[1],fout);
                                                fprintf(fout,"\tsavebp\n");
												fprintf(fout,"\tinc\t%d\n",table_nbre_formels[n->t_oper.op[0]->t_identifier.index]);
												fprintf(fout,"\tcall\t%s\n",n->t_oper.op[0]->t_identifier.ident);
												fprintf(fout,"\tdec\t%d\n",table_nbre_formels[n->t_oper.op[0]->t_identifier.index]);
												fprintf(fout,"\trstrbp\n");
												fprintf(fout,"\tdec\t%d\n",table_nbre_formels[n->t_oper.op[0]->t_identifier.index]);
                                              
                                                break;
                                        case OPER_ADD:
                                                generateAsmRec(n->t_oper.op[0],fout);
                                                if(n->t_oper.op[0]->type==typeIdentifier)
                                                    fprintf(fout,"\tmts\n");
                                                generateAsmRec(n->t_oper.op[1],fout);
                                                if(n->t_oper.op[1]->type==typeIdentifier)
                                                    fprintf(fout,"\tmts\n");
    
                                                fprintf(fout,"\tadd\n");
                                                break;
                                        case OPER_SUB:
                                                generateAsmRec(n->t_oper.op[0],fout);
                                                if(n->t_oper.op[0]->type==typeIdentifier){
                                                    fprintf(fout,"\tmts\n");
                                                }
                                                generateAsmRec(n->t_oper.op[1],fout);
                                                if(n->t_oper.op[1]->type==typeIdentifier){
                                                    fprintf(fout,"\tmts\n");
                                                }
                                                fprintf(fout,"\tsub\n");
                                                break;
                                        case OPER_MULT:
                                                generateAsmRec(n->t_oper.op[0],fout);
                                                if(n->t_oper.op[0]->type==typeIdentifier){
                                                    fprintf(fout,"\tmts\n");
                                                }
                                                generateAsmRec(n->t_oper.op[1],fout);
                                                if(n->t_oper.op[1]->type==typeIdentifier){
                                                    fprintf(fout,"\tmts\n");
                                                }
                                                fprintf(fout,"\tmult\n");
                                                break;
                                        case OPER_DIV:
                                                generateAsmRec(n->t_oper.op[0],fout);
                                                if(n->t_oper.op[0]->type==typeIdentifier){
                                                    fprintf(fout,"\tmts\n");
                                                }
                                                generateAsmRec(n->t_oper.op[1],fout);
                                                if(n->t_oper.op[1]->type==typeIdentifier){
                                                    fprintf(fout,"\tmts\n");
                                                }
                                                fprintf(fout,"\tdiv\n");
                                                break;
                                        case OPER_INF:
                                                generateAsmRec(n->t_oper.op[0],fout);
                                                if(n->t_oper.op[0]->type==typeIdentifier){
                                                    fprintf(fout,"\tmts\n");
                                                }

                                                generateAsmRec(n->t_oper.op[1],fout);
                                                if(n->t_oper.op[1]->type==typeIdentifier){
                                                    fprintf(fout,"\tmts\n");
                                                }
                                                
                                                fprintf(fout,"\tjg\tendlabel%d\n",currentLabel-1);
                                                break;
                                        case OPER_SUP:
                                                generateAsmRec(n->t_oper.op[0],fout);
                                                if(n->t_oper.op[0]->type==typeIdentifier){
                                                    fprintf(fout,"\tmts\n");
                                                }

                                                generateAsmRec(n->t_oper.op[1],fout);
                                                if(n->t_oper.op[1]->type==typeIdentifier){
                                                    fprintf(fout,"\tmts\n");
                                                }
                                               
                                                fprintf(fout,"\tjl\tendlabel%d\n",currentLabel-1);
                                                break;
                                        case OPER_EQ:
                                                generateAsmRec(n->t_oper.op[0],fout);
                                                if(n->t_oper.op[0]->type==typeIdentifier){
                                                    fprintf(fout,"\tmts\n");
                                                }

                                                generateAsmRec(n->t_oper.op[1],fout);
                                                if(n->t_oper.op[1]->type==typeIdentifier){
                                                    fprintf(fout,"\tmts\n");
                                                }
                                                
                                                fprintf(fout,"\tjl\tendlabel%d\n",currentLabel-1);
                                                fprintf(fout,"\tjg\tendlabel%d\n",currentLabel-1);
                                                break;
                                        case OPER_NE:
                                                generateAsmRec(n->t_oper.op[0],fout);
                                                generateAsmRec(n->t_oper.op[1],fout);
                                               
                                                fprintf(fout,"\tne\n");
                                                break;
                                        case OPER_NOT:
                                                generateAsmRec(n->t_oper.op[0],fout);
                                                generateAsmRec(n->t_oper.op[1],fout);
                                               
                                                fprintf(fout,"\tnot\n");
                                                break;
                                        case OPER_SKIP:
                                                printf("OPER_SKIP \n");
                                                generateAsmRec(n->t_oper.op[0],fout);
                                                break;
                                        case OPER_RETURN:
                                                //generateAsmRec(n->t_oper.op[0],fout);
                                                //generateAsmRec(n->t_oper.op[1],fout);
                                                //fprintf(fout,"\tmove\t1\n");
                                                //fprintf(fout,"\tret\n");
                                               
                                                break;
                                        case OPER_WRITE:
                                                printf("affichage\n");
                                                generateAsmRec(n->t_oper.op[0],fout);
                                                if(n->t_oper.op[0]->type==typeIdentifier){
                                                    
                                                    fprintf(fout,"\tmts\n");
                                                }
                                                //fprintf(fout,"\tmts\n");
                                                fprintf(fout,"\toutput\n");
                                                break;
                                        case OPER_DEF_FONCTION:
                                        		
                                        		crt++;
                                        		nbpf=table_nbre_formels[crt];
                                                fprintf(fout,"%s:\n",table_ident_fonctions[0][crt].ident);
                                                //fprintf(fout,"%s::\n",table_noms_fonctions[crt]);
                                                fprintf(fout,"\tlibp\t%d\n",-nbpf-2);
                                               // generateAsmRec(n->t_oper.op[0],fout);
                                                printf("appel fonction generateAsmFct\n");
                                                generateAsmFct(n->t_oper.op[1],fout);
                                                //printf("fin appel fonction generateAsmFct\n");
                                                //printf("définition de fonction\n");
                                                break;
                                        case OPER_ASSIGN:
                                                //generateAsmRec(n->t_oper.op[0],fout);
                                        		fprintf(fout,"\tpush\t%d\n",n->t_oper.op[0]->t_identifier.index);
                                                generateAsmRec(n->t_oper.op[1],fout);                                            
                                                fprintf(fout,"\tstm\n");
                                                
                                       
                                                break;
                                        case OPER_WHILE:
                                                
                                                label1=currentLabel;
                                                fprintf(fout,"label%d:\n",currentLabel);
                                                currentLabel++;
                                                generateAsmRec(n->t_oper.op[0],fout);
                                                generateAsmRec(n->t_oper.op[1],fout);
                                                fprintf(fout,"\tjp\tlabel%d\n",label1);
                                                fprintf(fout,"endlabel%d:\n",label1);
                                               
                                                
                                                printf("boucle while\n");
                                                break;
                                        case OPER_IF:
                                               label1=currentLabel;
                                               fprintf(fout,"if%d:\n",currentLabel);
                                               currentLabel++; 
                                               generateAsmRec(n->t_oper.op[0],fout);
                                               generateAsmRec(n->t_oper.op[1],fout);
                                               fprintf(fout,"\tjp\tlabel%d\n",label1);
                                               fprintf(fout,"endlabel%d:\n",label1);
                                    if (n->t_oper.nOperands==3)
                                                    generateAsmRec(n->t_oper.op[2],fout);
                                                fprintf(fout,"label%d:\n",label1);
                        
                                        
                                                break;
                                        case OPER_SEQUENCE:
                                        		
                                                generateAsmRec(n->t_oper.op[0],fout);
                                                if(n->t_oper.op[0]->type==typeIdentifier){
                                                    
                                                    fprintf(fout,"\tmts\n");
                                                }

                                                generateAsmRec(n->t_oper.op[1],fout);
                                               
                                                break;
                                        case OPER_RESERVE_SPACE:
                                                
                                                break;
                                        case OPER_MAIN:
                                                
                                                fprintf(fout,"main:\n");
                                                // inc des variable global
                                                fprintf(fout,"\tinc\t%d\n",table_nbre_variables_globales[0]);
                                                generateAsmRec(n->t_oper.op[0],fout);

                                                break;
                                        default:
                                                break;
                                }
                        }
                        break;
        }

    

}

void generateAsmExpression(nodeType *n, FILE *fout)
{
        if (n==NULL)
                return;

}



void generateAsmFct(nodeType *n,FILE *fout){
	int label1,label2,index;	

	//printf("\nassssmmmfonction\n");
	if (n==NULL)
		return;
 	printf("\nassssmmmfonction\n");
    switch (n->type)
        {
                case typeNumeric:
                        {
                            
                            fprintf(fout,"\tpushr\t%3.f\n",n->t_numeric.valeur);
                            printf("pushr ---def fonction\n");
                        }
                        break;
                case typeIdentifier:
                        {
               				if(n->t_identifier.index>=nbpf){
                                        			index=n->t_identifier.index-nbpf ;
                                        		}else{
                                        			index=-nbpf+n->t_identifier.index-1;
                                        		}
                           	
                            fprintf(fout,"\tlibp\t%d\n",index);
                            printf("libp ---def fonction\n");

                        }
                        break;
                case typeOperator:
                        {
                                switch (n->t_oper.oper)
                                {
                                        case OPER_APPEL_FONCTION:
                                                //generateAsmRec(n->t_oper.op[0],fout);
                                                generateAsmFct(n->t_oper.op[1],fout);
                                                fprintf(fout,"\tsavebp\n");
												fprintf(fout,"\tinc\t%d\n",table_nbre_formels[n->t_oper.op[0]->t_identifier.index]);
												fprintf(fout,"\tcall\t%s\n",n->t_oper.op[0]->t_identifier.ident);
												fprintf(fout,"\tdec\t%d\n",table_nbre_formels[n->t_oper.op[0]->t_identifier.index]);
												fprintf(fout,"\trstrbp\n");
												fprintf(fout,"\tdec\t%d\n",table_nbre_formels[n->t_oper.op[0]->t_identifier.index]);
                                              
                                                printf("appel fonction branche def fonction\n");
                                                break;
                                        case OPER_ADD:
                                                generateAsmFct(n->t_oper.op[0],fout);
                                                if(n->t_oper.op[0]->type==typeIdentifier){
                                                	
                                                    fprintf(fout,"\tcopy\t1\n");
                                                }
                                                generateAsmFct(n->t_oper.op[1],fout);
                                                if(n->t_oper.op[1]->type==typeIdentifier){  	
                                                    fprintf(fout,"\tcopy\t1\n");
                                                }
    
                                                fprintf(fout,"\tadd\n");
                                                break;
                                        case OPER_SUB:
                                                generateAsmFct(n->t_oper.op[0],fout);
                                                if(n->t_oper.op[0]->type==typeIdentifier){
                                                	
                                                    fprintf(fout,"\tcopy\t1\n");
                                                }
                                                generateAsmFct(n->t_oper.op[1],fout);
                                                if(n->t_oper.op[1]->type==typeIdentifier){  	
                                                    fprintf(fout,"\tcopy\t1\n");
                                                }
    
                                                fprintf(fout,"\tsub\n");
                                                break;
                                        case OPER_MULT:
                                                generateAsmFct(n->t_oper.op[0],fout);
                                                if(n->t_oper.op[0]->type==typeIdentifier){
                                                	
                                                    fprintf(fout,"\tcopy\t1\n");
                                                }
                                                generateAsmFct(n->t_oper.op[1],fout);
                                                if(n->t_oper.op[1]->type==typeIdentifier){  	
                                                    fprintf(fout,"\tcopy\t1\n");
                                                }
    
                                                fprintf(fout,"\tmult\n");
                                                break;
                                        case OPER_DIV:
                                                generateAsmFct(n->t_oper.op[0],fout);
                                                if(n->t_oper.op[0]->type==typeIdentifier){
                                                	
                                                    fprintf(fout,"\tcopy\t1\n");
                                                }
                                                generateAsmFct(n->t_oper.op[1],fout);
                                                if(n->t_oper.op[1]->type==typeIdentifier){  	
                                                    fprintf(fout,"\tcopy\t1\n");
                                                }
    
                                                fprintf(fout,"\tdiv\n");
                                                break;
                                        case OPER_INF:
                                                generateAsmFct(n->t_oper.op[0],fout);
                                                if(n->t_oper.op[0]->type==typeIdentifier)
                                                    fprintf(fout,"\tcopy\t1\n");

                                                generateAsmFct(n->t_oper.op[1],fout);
                                                if(n->t_oper.op[1]->type==typeIdentifier)
                                                    fprintf(fout,"\tcopy\t1\n");
                                                printf("OPER_INF\n");
                                                fprintf(fout,"\tjg\tendlabel%d\n",currentLabel-1);
                                                break;
                                        case OPER_SUP:
                                                generateAsmFct(n->t_oper.op[0],fout);
                                                if(n->t_oper.op[0]->type==typeIdentifier)
                                                    fprintf(fout,"\tcopy\t1\n");

                                                generateAsmFct(n->t_oper.op[1],fout);
                                                if(n->t_oper.op[1]->type==typeIdentifier)
                                                    fprintf(fout,"\tcopy\t1\n");
                                                printf("OPER_sup\n");
                                                fprintf(fout,"\tjl\tendlabel%d\n",currentLabel-1);
                                                break;
                                        case OPER_EQ:
                                               /* generateAsmRec(n->t_oper.op[0],fout);
                                                if(n->t_oper.op[0]->type==typeIdentifier)
                                                    fprintf(fout,"\tmts\n");

                                                generateAsmRec(n->t_oper.op[1],fout);
                                                if(n->t_oper.op[1]->type==typeIdentifier)
                                                    fprintf(fout,"\tmts\n                                                printf("OPER_ok\n");
                                                fprintf(fout,"\tjl\tendlabel%d\n",currentLabel-1);
                                                fprintf(fout,"\tjg\tendlabel%d\n",currentLabel-1);*/
                                                break;
                                        case OPER_NE:
                                                //generateAsmRec(n->t_oper.op[0],fout);
                                                //generateAsmRec(n->t_oper.op[1],fout);
                                                printf("OPER_NE def fonction\n");
                                                fprintf(fout,"\tne\n");
                                                break;
                                        case OPER_NOT:
                                                //generateAsmRec(n->t_oper.op[0],fout);
                                                //generateAsmRec(n->t_oper.op[1],fout);
                                                printf("OPER_NOT\n");
                                                fprintf(fout,"\tnot\n");
                                                break;
                                        case OPER_SKIP:
                                                printf("OPER_SKIP \n");
                                                //generateAsmRec(n->t_oper.op[0],fout);
                                                break;
                                        case OPER_RETURN:
                                                //generateAsmRec(n->t_oper.op[0],fout);
                                               
                                                
                                                //printf("retour dans la fonction generateAsmRec\n");
                                                generateAsmFct(n->t_oper.op[1],fout);
                                                fprintf(fout,"\tmove\t1\n");
                                                fprintf(fout,"\tret\n");
                                                break;
                                        case OPER_WRITE:
                                                printf("affichage\n");
                                                generateAsmFct(n->t_oper.op[0],fout);
                                                if(n->t_oper.op[0]->type==typeIdentifier){                                               
                                                    fprintf(fout,"\tcopy\t1\n");
                                                }
                                                //fprintf(fout,"\tmts\n");
                                                fprintf(fout,"\toutput\n");
                                                printf("write def fonction\n");
                                                break;
                                        
                                        case OPER_ASSIGN:
                                        		
                                        		if(n->t_oper.op[0]->t_identifier.index>=nbpf){
                                        			index=n->t_oper.op[0]->t_identifier.index-nbpf ;
                                        		}else{
                                        			index=-nbpf+n->t_oper.op[0]->t_identifier.index-1;
                                        		}
                                                //generateAsmFct(n->t_oper.op[0],fout);
                                        		fprintf(fout,"\tlibp\t%d\n",index);
                                        		printf("ciicsi %d---%d----%s\n",-table_nbre_formels[crt]+n->t_oper.op[0]->t_identifier.index-1,n->t_oper.op[0]->t_identifier.index,n->t_oper.op[0]->t_identifier.ident);
                                                generateAsmFct(n->t_oper.op[1],fout);
                                                printf("assignation de variable  def fonction\n");
                                                
                                                fprintf(fout,"\tmove\t1\n");
                                                
                                       
                                                break;
                                        case OPER_WHILE:
                                                
                                                label1=currentLabel;
                                                fprintf(fout,"label%d:\n",currentLabel);
                                                currentLabel++;
                                                generateAsmFct(n->t_oper.op[0],fout);
                                                generateAsmFct(n->t_oper.op[1],fout);
                                                fprintf(fout,"\tjp\tlabel%d\n",label1);
                                                fprintf(fout,"endlabel%d:\n",label1);
                                               
                                                
                                                printf("boucle while def_fonction\n");
                                                break;
                                        case OPER_IF:
                                               label1=currentLabel;
                                               fprintf(fout,"if%d:\n",currentLabel);
                                               currentLabel++; 
                                               generateAsmFct(n->t_oper.op[0],fout);
                                               generateAsmFct(n->t_oper.op[1],fout);
                                               fprintf(fout,"\tjp\tlabel%d\n",label1);
                                               fprintf(fout,"endlabel%d:\n",label1);
                                    if (n->t_oper.nOperands==3)
                                                    generateAsmFct(n->t_oper.op[2],fout);
                                                fprintf(fout,"label%d:\n",label1);
                        
                                        printf("instruction if def_fonction\n");
                                                break;
                                        case OPER_SEQUENCE:
                                                generateAsmFct(n->t_oper.op[0],fout);
                                                if(n->t_oper.op[0]->type==typeIdentifier){
                                                    
                                                    fprintf(fout,"\tcopy\t1\n");
                                                }
   
                                                generateAsmFct(n->t_oper.op[1],fout);
                                                printf("OPER_SEQUENCE def_fonction\n");
                                                break;
                                        
                                        default:
                                                break;
                                }
                        }
                        break;
        }
}

void generateAsm(nodeType *n, char *filename)
{
	FILE *fout;

	currentLabel=0;
	fout=fopen(filename,"w");
	fprintf(fout,"\tjp\tmain\n");
	generateAsmRec(n,fout);

	fprintf(fout,"\thalt\n");
	fprintf(fout,"\tend\n");
	fclose(fout);	
}

void generateDigraphNameNode(nodeType *n,FILE *fout)
{

        if (n==NULL)
                return;

        switch (n->type)
        {
                case typeNumeric:
                        {
				n->digraphNode=countDigraph++;
                                fprintf(fout,"\tA%3.3d [label=\"%f\"]\n",n->digraphNode,n->t_numeric.valeur);
                        }
                        break;
                case typeIdentifier:
                        {
				n->digraphNode=countDigraph++;
                                fprintf(fout,"\tA%3.3d [label=\"%s\"]\n",n->digraphNode,n->t_identifier.ident);
                        }
                        break;
                case typeOperator:
                        {
                                switch (n->t_oper.oper)
                                {
                                        case OPER_APPEL_FONCTION:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
                                                n->digraphNode=countDigraph++;
                                                fprintf(fout,"\tA%3.3d [label=\"appel fonc\"]\n",n->digraphNode);
                                                break;
                                        case OPER_ADD:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
                                                n->digraphNode=countDigraph++;
                                                fprintf(fout,"\tA%3.3d [label=\"+\"]\n",n->digraphNode);
                                                break;
                                        case OPER_SUB:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
                                                n->digraphNode=countDigraph++;
                                                fprintf(fout,"\tA%3.3d [label=\"-\"]\n",n->digraphNode);
                                                break;
                                        case OPER_MULT:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
                                                n->digraphNode=countDigraph++;
                                                fprintf(fout,"\tA%3.3d [label=\"*\"]\n",n->digraphNode);
                                                break;
                                        case OPER_DIV:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
                                                n->digraphNode=countDigraph++;
                                                fprintf(fout,"\tA%3.3d [label=\"/\"]\n",n->digraphNode);
                                                break;
                                        case OPER_INF:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
                                                n->digraphNode=countDigraph++;
                                                fprintf(fout,"\tA%3.3d [label=\"<\"]\n",n->digraphNode);
                                                break;
                                        case OPER_SUP:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
                                                n->digraphNode=countDigraph++;
                                                fprintf(fout,"\tA%3.3d [label=\">\"]\n",n->digraphNode);
                                                break;
                                        case OPER_EQ:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
                                                n->digraphNode=countDigraph++;
                                                fprintf(fout,"\tA%3.3d [label=\"==\"]\n",n->digraphNode);
                                                break;
                                        case OPER_NE:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
                                                n->digraphNode=countDigraph++;
                                                fprintf(fout,"\tA%3.3d [label=\"!=\"]\n",n->digraphNode);
                                                break;
                                        case OPER_NOT:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                n->digraphNode=countDigraph++;
                                                fprintf(fout,"\tA%3.3d [label=\"not\"]\n",n->digraphNode);
                                                break;
                                        case OPER_SKIP:
						n->digraphNode=countDigraph++;
                                		fprintf(fout,"\tA%3.3d [label=\"skip\"]\n",n->digraphNode);
                                                break;
                                        case OPER_RETURN:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
						n->digraphNode=countDigraph++;
                                		fprintf(fout,"\tA%3.3d [label=\"return\"]\n",n->digraphNode);
                                                break;
                                        case OPER_WRITE:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
						n->digraphNode=countDigraph++;
                                		fprintf(fout,"\tA%3.3d [label=\"write\"]\n",n->digraphNode);
                                                break;
                                        case OPER_DEF_FONCTION:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
						n->digraphNode=countDigraph++;
                                		fprintf(fout,"\tA%3.3d [label=\"def_fonction\"]\n",n->digraphNode);
                                                break;
                                        case OPER_ASSIGN:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
						n->digraphNode=countDigraph++;
                                		fprintf(fout,"\tA%3.3d [label=\"=\"]\n",n->digraphNode);
                                                break;
                                        case OPER_WHILE:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
						n->digraphNode=countDigraph++;
                                		fprintf(fout,"\tA%3.3d [label=\"while\"]\n",n->digraphNode);
                                                break;
                                        case OPER_IF:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
						if (n->t_oper.nOperands==3)
                                                	generateDigraphNameNode(n->t_oper.op[2],fout);
						n->digraphNode=countDigraph++;
                                		fprintf(fout,"\tA%3.3d [label=\"if\"]\n",n->digraphNode);
                                                break;
                                        case OPER_SEQUENCE:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
                                                generateDigraphNameNode(n->t_oper.op[1],fout);
						n->digraphNode=countDigraph++;
                                		fprintf(fout,"\tA%3.3d [label=\";\"]\n",n->digraphNode);
                                                break;
                                        case OPER_RESERVE_SPACE:
						n->digraphNode=countDigraph++;
                                		fprintf(fout,"\tA%3.3d [label=\"reserve_space\"]\n",n->digraphNode);
                                                break;
                                        case OPER_MAIN:
                                                generateDigraphNameNode(n->t_oper.op[0],fout);
						n->digraphNode=countDigraph++;
                                		fprintf(fout,"\tA%3.3d [label=\"main\"]\n",n->digraphNode);
                                                break;
                                        default:
                                                break;
                                }
                        }
                        break;
        }

}

void generateDigraphEdges(nodeType *n,FILE *fout)
{
        if (n==NULL)
                return;

        switch (n->type)
        {
                case typeNumeric:
                case typeIdentifier:
                        break;
                case typeOperator:
                        {
				//printf("oper=%d\n",n->t_oper.oper);
                                switch (n->t_oper.oper)
                                {
                                        case OPER_ADD:
                                        case OPER_SUB:
                                        case OPER_MULT:
                                        case OPER_DIV:
                                        case OPER_INF:
                                        case OPER_SUP:
                                        case OPER_EQ:
                                        case OPER_NE:
                                        case OPER_WHILE:
                                        case OPER_APPEL_FONCTION:
                                        case OPER_DEF_FONCTION:
                                        case OPER_ASSIGN:
                                        case OPER_SEQUENCE:
                                        case OPER_RETURN:
                                                fprintf(fout,"\tA%3.3d -> A%3.3d\n", n->digraphNode,n->t_oper.op[0]->digraphNode);
                                                fprintf(fout,"\tA%3.3d -> A%3.3d\n", n->digraphNode,n->t_oper.op[1]->digraphNode);
                                                generateDigraphEdges(n->t_oper.op[0],fout);
                                                generateDigraphEdges(n->t_oper.op[1],fout);
                                                break;
                                        case OPER_IF:
                                                fprintf(fout,"\tA%3.3d -> A%3.3d\n", n->digraphNode,n->t_oper.op[0]->digraphNode);
                                                fprintf(fout,"\tA%3.3d -> A%3.3d\n", n->digraphNode,n->t_oper.op[1]->digraphNode);
						if (n->t_oper.nOperands==3)
                                                	fprintf(fout,"\tA%3.3d -> A%3.3d\n", n->digraphNode,n->t_oper.op[2]->digraphNode);
                                                generateDigraphEdges(n->t_oper.op[0],fout);
                                                generateDigraphEdges(n->t_oper.op[1],fout);
						if (n->t_oper.nOperands==3)
                                                	generateDigraphEdges(n->t_oper.op[2],fout);
                                                break;
                                        case OPER_WRITE:
                                        case OPER_NOT:
					case OPER_MAIN:
                                                fprintf(fout,"\tA%3.3d -> A%3.3d\n", n->digraphNode,n->t_oper.op[0]->digraphNode);
                                                generateDigraphEdges(n->t_oper.op[0],fout);
                                                break;
					case OPER_SKIP:
					case OPER_RESERVE_SPACE:
						break;
                                        default:
                                                break;
                                }
                        }
                        break;
        }
}

void generateDigraph(nodeType *n)
{

	FILE *fout;

	fout=fopen("res.dot","w+");
	countDigraph=0;
	fprintf(fout,"digraph {\n");
	printf("generateDigraphNameNode\n");
	generateDigraphNameNode(n,fout);
	printf("generateDigraphEdges\n");
	generateDigraphEdges(n,fout);
	fprintf(fout,"}\n");
	fclose(fout);
	system("dot -Tpng res.dot -o res.png");

}
