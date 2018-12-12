# Tutorial do Code Genererator
---

## 1 - Tela inicial
A tela inicial do assitente Code Generator é apresentada na imagem a seguir.

![](https://i.ibb.co/Pm901GD/1-bemvindo.png)

- Para prosseguir o usuário deve clicar no botão _**Avançar**_.

## 2 - Tela para carregar configurações salvas
A tela seguinte permite que o usuário escolha se deseja carregar as informações salvas ou não.

![](https://i.ibb.co/fYvH5sr/2-configuracoessalvas.png)

- Caso o usuário deseje carregar as informações salvas ele deve clicar no botão _**Carregar**_.

- Em seguida o usuário deve clicar no botão _**Avançar**_.

## 3 - Tela para selecionar arquivos de entrada
Na tela seguinte o usuário deve informar os arquivos de código fonte desenvolvidos através do software **Busmaster**. Para auxiliar a preencher o nome destes arquivos, pode se utilizar o explorador de arquivos através do botão _**Procurar...**_ associado a cada campo de preenchimento.

![](https://i.ibb.co/5Y7cVR9/3-arquivosbusmaster.png)

- Para selecionar o arquivo de definições pode-se clicar no botão _**Procurar...**_ a ele associado. 
![](https://i.ibb.co/m5RZHrX/3-1-arquivo-def.png)

- Para selecionar o arquivo do banco de dados de mensagens pode-se fazer o mesmo. 
![](https://i.ibb.co/mzmsjWf/3-2-arquivo-dbf.png)

- Para selecionar o arquivo de código fonte _C++_ também pode-se fazer o mesmo. 
![](https://i.ibb.co/w7jGqhK/3-3-arquivo-cpp.png)

- Ao concluir o preenchimento dos campos o usuário deve clicar em _**Avançar**_ para prosseguir.

## 4 - Tela para configurações do TrampolineRTOS
Na tela seguinte o usuário deve informar o diretório principal do **TrampolineRTOS** e o caminho para o executável do **GOIL**. Para auxiliar a preencher o os caminhos, pode se utilizar o explorador de arquivos através do botão _**Procurar...**_ associado a cada campo de preenchimento.

![](https://i.ibb.co/ZJv1rPR/4-configuracoestrampoline.png)

- Para preencher o caminho do diretório base do **TrampolineRTOS** o usuário pode clicar no botão _**Procurar...**_.
![](https://i.ibb.co/khQxmfd/4-1-diretoriotrampoline.png)

- Para preencher o caminho do executável do **GOIL** o usuário pode clicar no botão _**Procurar...**_.
![](https://i.ibb.co/jfwsPp0/4-2-executavelgoil.png)

- Ao concluir o preenchimento dos campos o usuário deve clicar em _**Avançar**_ para prosseguir.

## 5 - Tela de configurações dos arquivos de saída
Na tela seguinte o usuário deve informar o diretório onde deve ser salvo os arquivos de código fonte e o binário produzidos. O usuário também deve informar o prefixo do nome dos arquivos produzidos.

![](https://i.ibb.co/vdwyTbW/5-configuracoessaida.png)

- Para preencher o caminho do diretório de saída o usuário pode clicar no botão _**Procurar...**_.
![](https://i.ibb.co/gtJFLw3/5-1-diretoriosaida.png)

- O usuário deve preencher o prefixo dos arquivos de saída manualmente e este prefixo não pode conter extensão.
![](https://i.ibb.co/6sw94dK/5-2-prefixo.png)

- Ao concluir o preenchimento dos campos o usuário deve clicar em _**Avançar**_ para prosseguir.

## 6 - Tela de configurações dos pinos
Na tela seguinte o usuário pode realizar o mapeamento das teclas associadas as funções _key handlers_ no **Busmaster** para pinos no microcontrolador. Também pode selecionar a plataforma de hardware utilizada, indicar o tipo de pino e caso o pino seja do tipo digital, informar qual nível lógico do pino corresponde ao estado ativo.
A figura da plataforma de hardware com a informação da pinagem serve para auxiliar o usuário no mapeamento de teclas para pinos.
Nesta tela, apenas aparecem apenas as teclas associada a funções dos _key handlers_ que foram extraídos dos arquivos de entrada.

![](https://i.ibb.co/fqsgTWn/6-configuracoespinos.png)

- Para selecionar a plataforma de hardware o usuário deve clicar no campo de seleção acima da imagem do arduino.
![](https://i.ibb.co/1dsZQ6m/6-1-arduino.png)

- Para selecionar o tipo de entrada deve-se clicar no campo de seleção do tipo de entrada para cada tecla e selecionar o tipo adequado (digital ou analógico).
![](https://i.ibb.co/C6cT5NV/6-2-tipoentrada.png)

- Para selecionar o pino associado a cada tecla deve-se clicar no campo de seleção do pino. Os pinos permitidos para seleção estão de acordo com o tipo de entrada (digital ou analógica). E os pinos digitais _"2,10,11,12,13"_ não podem ser selecionados, pois são utilizados na ligação com o módulo MCP2515.
![](https://i.ibb.co/v3MtKjP/6-3-pino.png)

- Para selecionar o nível lógico do pino digital que corresponde ao estado ativo para cada pino deve-se clicar no campo de seleção e escolher entre _"1 (alto)"_ ou _"0 (baixo)"_. 
![](https://i.ibb.co/PjK5DYK/6-4-nivellogico.png)

- Ao concluir o preenchimento dos campos o usuário deve clicar em _**Avançar**_ para prosseguir.

## 7 - Tela de configurações do código para a plataforma de hardware
Na tela seguinte o usuário pode realizar uma série de configurações a respeito do código fonte produzido para a plataforma de hardware. 
O usuário pode informar se a ECU em questão envia mensagens pelo barramento CAN, possibilitando a instanciação da fila de envio de mensagens e da _task_ responsável pelo envio.
O usuário pode informar se a ECU em questão faz uso da interface serial, para que a mesma possa ser devidamente inicializada.
O usuário deve informar o endereço da ECU em base hexadecimal (0 a FF).
O usuário pode informar o tamanho da pilha de controle, em valor decimal e múltiplo de 2, de cada _task_ que será instanciada pela aplicação. Entretanto, cada _task_ possui um tamanho de pilha de controle pré-definido e que será utilizado caso o usuário não deseje especificá-lo. Os tamanhos pré-definidos das pilhas de controle das _tasks_ são:
  * Para a can_send_task o tamanho padrão é de 128 bytes.
  * Para a pins_reader_task o tamanho padrão é de 128 bytes.
  * Para a can_recv_task o tamanho padrão é de 256 bytes.
  * Para qualquer timer_task o tamanho padrão é de 128 bytes.

![](https://i.ibb.co/GxpFFNR/7-configuracoescodigoplataforma.png)

- Na Figura a seguir observamos um exemplo de preenchimento dos campos.
![](https://i.ibb.co/VMSwTNw/7-1-valorespreenchidos.png)

- Ao concluir o preenchimento dos campos o usuário deve clicar em _**Avançar**_ para prosseguir.

## 8 - Tela com o resultado da geração de código e compilação
Na tela seguinte o usuário pode observar os resultados da geração de código e compilação.

![](https://i.ibb.co/N2D6JtP/8-geracaoecompilacao.png)

- Ao concluir o preenchimento dos campos o usuário deve clicar em _**Avançar**_ para prosseguir.

## 9 - Tela para o carregamento do executável
Na tela seguinte o usuário pode informar a porta serial a qual o arduino está conectado através da caixa de seleção.

![](https://i.ibb.co/2P5MygW/9-carregarexecutavel.png)

- Ao concluir o preenchimento dos campos o usuário deve clicar em _**Avançar**_ para prosseguir.

## 10 - Tela com o resultado do carregamento
Na tela seguinte o usuário pode observar o resultado do carregamento do código executável para a plataforma de hardware.

![](https://i.ibb.co/0C9Rgvy/10-carregamento.png)

- Ao concluir o preenchimento dos campos o usuário deve clicar em _**Avançar**_ para prosseguir.

## 11 - Tela final
Na útlima tela o usuário pode clicar em _**Finalizar**_ caso deseje encerrar ou clicar no botão _**Configurar nova ECU**_ caso deseje prosseguir com a configuração de uma nova ECU.

![](https://i.ibb.co/dPXjZ98/11-fim.png)

---

## Dependências
O assistente Code Generator faz uso do **TrampolineRTOS**, **Busmaster**, **Toolchain do AVR**, **Python** dentre outras ferramentas. Portanto é necessário que cada uma delas esteja devidamente configurada no ambiente.
  - Devem estar presentes na variável de ambiente **Path** os diretórios de binários da toolchain do AVR e o diretório do executável do Python. Exemplo: ![](https://i.ibb.co/44YC6w0/path.png)

  - Para que os scripts Python possam ser executados através do assistente é necessário a definição da variável de ambiente **PYTHONPATH** com os diretórios internos ao diretório base da instalação do Python. Exemplo: ![](https://i.ibb.co/Y2G1kc3/pythonpath.png)

  - Para utilizar a biblioteca MCP_CAN é necessário que ela esteja localizada no caminho **$TRAMPOLINE\_ROOT\_DIR$/machines/avr/arduino/libraries/mcp_can** e que esteja corretamente configurada no trampoline. Para isso, é necessário que no arquivo **$TRAMPOLINE\_ROOT\_DIR$/goil/templates/config/avr/arduino/config.oil** seja adicionada as configurações da biblioteca.
  ![config.oil](https://i.ibb.co/yqcnjsW/config.png)
