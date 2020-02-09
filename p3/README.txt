# Alunos: Dimitre Ortt & Willian Ferreira


#-----------------------------------------##------------------------------------------#

A1 - Completamos a classe Light com os atributos referentes às luzes Directional, 
Spot e Point, a edição de tais atributos está disponível na janela inspector, são eles: 

 - position, 
 - direction (Spot, Direcional), 
 - falloff (Spot, Point),
 - ghama (ângulo de abertura da Spot),
 - color,
 - type,
 - fallExponent (expoente de decaimento da Spot).

#-----------------------------------------##------------------------------------------#

A2 - Implementamos o botão de adição e deleção de objetos de cena na janela da 
hierarquia. É possível adicionar luzes com seus respectivos tipos. No menu se encontra
3 opções para criar luzes: Point Light, Directional Light e Spot Light. As opções, 
como o nome sugere, criam objetos de cena adicionando componentes do tipo luz, o
qual tem o tipo correspondente.

#-----------------------------------------##------------------------------------------#

A3 - Implementamos em P3.cpp o método P3::drawLight(), o qual recebe uma referência 
para um componente Light e desenha na janela o desenho correspondente. Cada tipo de 
luz possui um desenho. As luzes Directional e Spot tem o desenho apontando para o foco
de sua luz.

#-----------------------------------------##------------------------------------------#

A4 - Implementamos um programa GLSL de tonalização de Gouraud com o modelo de 
iluminação de phong nos programas gouraud.vs e gouraud.fs. Este é o programa sendo 
usado para a renderização da cena no modo de edição.
O modelo de iluminação de phong o qual possui fórmula descrita e detalhadamente 
explicada no capítulo 4 está programado em OpenGL no arquivo gouraud.vs.

#-----------------------------------------##------------------------------------------#

A5 - Implementamos um programa GLSL de tonalização de Phong com o modelo de 
iluminação de phong nos programas phong.vs e phong.fs. Este é o programa sendo 
usado para a renderização da cena na janelinha de preview da camera corrente (caso
haja) e da cena no modo renderização.
O modelo de iluminação de phong está programado em OpenGL no arquivo phong.fs.

#-----------------------------------------##------------------------------------------#

A6 - Criamos uma cena inicial, renderizada assim que o programa entra em execução, a 
qual mostra objetos de cena e luzes dispostos em posições que mostrem o funcionamento
das luzes e a difusão das luzes pelos objetos.
- Criamos uma luz spot vermelha, uma luz Point verde e uma luz Directional azul.
- Criamos 3 boxes flats, dispostas como se formando uma sala, para que os raios de luz
se encontrem com as boxes e assim seja possível perceber o seu efeito. 
- Criamos uma camera (Main Camera), e a posicionamos de maneira a possuir a cena
em seu volume de vista. Colocamos esta câmera como o objeto corrente da cena no início
da execução, assim, na primeira renderização a câmera já é desenhada na tela assim
como sua janelinha de preview. Como a renderização na janelinha utiliza a tonalização
de Phong, podemos visualizar o seu funcionamento na primeira cena renderizada.

-#--#--#--#--#--#--#--#--#--#--#--#--#--#--#--#--#--#--#--#--#--#--#--#--#--#--#--#--#-


-#- Decisões de projeto:

 - No botão Add Component, na opção Primitive, é adicionado um primitivo com a malha 
	Box, o qual pode posteriormente ser alterado através do botão meshes.
 - Adição das luzes nos shaders: método P3::establishLights(): para cada luz presente 
	na hierarquia de objetos de cena, este método adiciona aos shaders, no vetor 
	lights[], os dados da luz. O vetor tem uma limitação de até 10 luzes. 
 - Permitimos a adição de luzes ilimitadas na hierarquia de objetos, porém na 
	renderização só serão utilizadas as 10 primeiras luzes ligadas encontradas na 
	hierarquia de objetos (percorrida de maneira in order).
 
-#- Observações:

 - O falloff é um inteiro que varia entre 0 e 2 inicializado com 1.
 - Na janela Inspector, ao inspecionar uma luz, criamos a checkbox "On", a qual
	determina se a luz está acesa ou não. 
 - Quando uma luz nova é criada atravéz do botão create, ela é iniciada com a cor 
	branca.
 - Não fizemos a entrega da Parte 2 do trabalho, assim, visto que as partes são 
	cumulativas, fizemos o P2 e o P3 para esta entrega, todas as funcionalidades 
	do P2 estão implementadas e funcionando adequadamente.
 - Nossos códigos das partes p1 e p2 não estão presentes nas pastas p1 e p2, estão 
	todos integrados diretamente nos códigos do P3.

-#- Recomendação durante os testes:

 - A cena é iniciada com 3 luzes acesas, para testar seu funcionamento, vá à janela
	inspector das luzes e apague uma ou duas, e então vá a janela da hierarquia
	e clique na Main Camera, o que mostrará a janelinha de preview da camera. 
	Assim, será possível visualizar os efeitos das luzes nas duas tonalizações 
	implementadas.