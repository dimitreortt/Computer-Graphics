# Computação Gráfica - Trabalho Prático Parte 4
# Alunos: Dimitre Ortt & Willian Ferreira
# Professor: Paulo Pagliosa

#-----------------------------------------##------------------------------------------#
A1 - Implementamos na classe RayTracer o método recursivo
	
	void hierarchyIntersection(const Ray& ray, Intersection& hit, SceneObject* o);

o qual é o responsável por verificar a intersecção de um raio com os objetos da
hierarquia da cena. Caso exista alguma intersecção, ele atribui o primitivo do objeto
cuja intersecção é a mais próxima ao atributo hit.object, além das informações sobre o
triângulo (mais próximo) interceptado. Este método funciona verificando a intersecção 
de um raio com cada objeto de cena e com seus filhos recursivamente atravéz do método
Primitive::intersect().

#-----------------------------------------##------------------------------------------#
A2 - Substituimos na classe Primitive a implementação do método Primitive::intersect() 
com a nossa implementação da intersecção de um raio com a malha de triângulos do 
primitivo. Nossa implementação funciona acessando-se cada triângulo da malha e 
calculando a intersecção do raio (local) com o triângulo através do algoritmo de 10 
linhas visto em sala (lab 1) no dia 5/11. 


#-----------------------------------------##------------------------------------------#
A3 - #

#-----------------------------------------##------------------------------------------#
A4 - Implementamos na classe RayTracer o método RayTracer::shade(), o qual é o 
responsável por tonalizar o ponto de intersecção. A tonalização de um ponto de 
intersecção é realizada em dois passos principais: o cálculo da iluminação direta 
através, dos raios de luz; cálculo da iluminação indireta através do raio de reflexão.

Para o cálculo da iluminação direta, estabelecemos na cena uma estrutura chamada 
_lights*, a qual armazena o endereço das luzes contidas dentre os objetos de cena da 
cena. Assim, para a iluminação direta, criamos um raio de luz para cada luz com origem
no ponto de intersecção e direcionado para a luz, caso o raio de luz intercepte algum
outro objeto da hierarquia, então este é um raio de sombra (cor preta), caso contrário
calculamos o efeito luminoso da luz no ponto através do modelo de iluminação de phong.

Para o cálculo da iluminação direta, criamos um raio de reflexão com origem no ponto
de intersecção e com a direção de reflexão do raio original, tal raio tem sua 
contribuição para a iluminação do ponto medida pela cor da intersecção deste com outro
ponto da hierarquia e assim recursivamente. Utilizamos para esta parte o algoritmo
explicado em sala (lab 3).

A iluminação de um ponto se dá então pela soma da iluminação direta com a iluminação
indireta.

#-----------------------------------------##------------------------------------------#
A5 - Criamos duas cenas para testar a implementação do traçado de raios. Para se 
selecionar a cena desejada criamos um menu na janela de exibição chamado "Scenes", 
este por sua vez contém duas opções selecionáveis, "Scene 1" e "Scene 2", a alteração
da cena corrente no menu causa a invocação do método P4::switchScenes(), o qual 
atualiza o atributo _currentScene de P4. Criamos três materiais: Hard, Soft e Curly e 
os utilizamos em nossos objetos da primeira cena, criamos também um material default, 
o qual é utilizado caso um primitivo não possua outra especificação de material. A 
seguir damos uma breve explicação das cenas:

Scene 1: A Scene 1 contém 7 objetos:
 - 1 luz pontual, posicionada próxima aos objetos, um pouco mais acima.
 - 4 boxes, uma grande para melhor se visualizar os outros objetos refletidos nela, 
	uma média que fará sombra nas outras e duas pequenas dispostas de maneira a 
	uma ser escondida da outra
 - 1 esfera, no centro da cena, para interferir nas reflexões e sombras, criando assim
	um bom efeito para se visualizar o funcionamento do traçado de raios. Esta 
	esfera tem um coeficiente specular definido como verde, assim fica mais fácil
	distinguir seus efeitos na reflexão.
 - 1 câmera, chamada Camera 0, cuja vista será renderizada com o traçado de raios.

Dispomos as boxes de maneira a ficar claro o começo e fim de suas sombrasque suas 
sombras. Deixamos a cor ambiente da cena um pouco mais escura, para que o efeito da 
luz fosse melhor percebido.

A direção default da câmera Scene Editor está contrária à direção da Camera 0, uma boa
maneira de se observar cena é caminhar um pouco com a Scene Editor através das teclas
de atalho, selecionar Camera 0 na hierarquia e visualizar sua posição, então 
selecionar outro objeto que não seja uma câmera para que a renderização funcione 
corretamente e então invocar o traçado de raios através do modo de vista de 
renderização.

Scene 2: A Scene 2 contém 6 objetos:
 - 2 luzes, uma pontual próxima aos objetos de cena para que seu efeito seja maior e
   e outra direcional, cuja direção é a mesma de eixo z.
 - 1 esfera média, 
 - 2 boxes, 1 grande para se analisar a reflexão da esfera nela e outra pequena ao lado
   da esfera para se analisar a reflexão no nível 2.
 - 1 câmera, chamada Camera 0, esta se encontra praticamente na mesma posição padrão
   da Scene Editor.

Nosso objeto com esta cena foi de posicionar a câmera voltada à parte de menos 
iluminação dos objetos e assim, ao renderizar, mostrar o funcionamento do algoritmo 
de cálculo dos raios secundários. Observamos nesta cena que a reflexão de nível 2 
partindo da esfera tem um funcionamento peculiar, ela parece ser realizada em uma 
direção de reflexão calculada à partir da normal invertida dos triângulos da malha
das esferas.


* Obs.: 
 - Notamos um problema em relação a reflexão das esferas,
   estas parecem calcular a iluminação secundária utilizando a normal invertida 
   nos cálculos, este problema não foi solucionado.

 - Se o traçado de raios for invocado enquanto uma câmera da cena está como objeto 
   corrente, a cena não será renderizada devidamente.
#-----------------------------------------##------------------------------------------#

#-----------------------------------------##------------------------------------------#

Observações:

 - No método P4::hieararchyWindow implementamos a remoção (sobrecarregada) de uma luz 
   da cena, a qual é invocada ao se deletar um objeto da cena que contenha uma luz.

 - Para a criação de duas cenas, objeto do A5, reformamos um pouco da estrutura da 
   classe P4, criamos o atributo Scene::_currentCamera, o qual substitui o atributo 
   Camera::_current, pois agora cada uma das cenas terá sua própria câmera corrente e 
   um atributo estático compartilhado entre cenas não funcionaria.

 - Tivemos um constante problema com a reflexão das esferas, colocamos uma esfera em 
   nossa cena, a qual não teve a luz de reflexão perfeita, porém deu-se para notar uma 
   reflexão próxima da ideal.