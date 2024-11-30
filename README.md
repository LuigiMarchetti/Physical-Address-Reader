# Leitor de Setores de Disco Físico

## Visão Geral
Aplicativo em linguagem C para Windows que permite a leitura e visualização detalhada de setores individuais em um disco físico. Desenvolvido para explorar o conteúdo interno de discos, o programa fornece uma interface gráfica intuitiva para navegação e análise de dados em múltiplos formatos.

## Recursos Principais
- Leitura de setores em disco físico
- Visualização em formatos hexadecimal, binário e ASCII
- Interface gráfica amigável
- Navegação entre setores
- Exibição de endereços físicos

## Requisitos
- Compilador C
- CMake 3.10+
- Windows SDK
- CLion ou outro ambiente de desenvolvimento C

## Instalação

### Método 1: CLion
1. Clone o repositório:
   ```bash
   git clone https://github.com/LuigiMarchetti/Physical-Address-Reader.git
1. Abra o projeto no CLion.
2. Configure o CMake com as opções padrão para Windows ou utilize as configurações fornecidas no repositório clonado.
3. Compile o projeto diretamente no IDE.


## Como Usar

1. Execute o aplicativo com privilégios de administrador.
2. Insira o número do setor desejado.
3. Clique no botão **"Read Sector"** para ler os dados.
4. Use os botões **"<"** e **">"** para navegar pelos setores adjacentes.

## Avisos de Segurança

- **Requer privilégios de administrador**: O acesso direto ao disco exige permissões elevadas.
- **Use com cuidado**: A leitura de discos físicos pode causar danos se usada de forma inadequada.
- **Não recomendado para produção**: Este aplicativo foi desenvolvido para fins educacionais e de exploração.

## Limitações

- Funciona exclusivamente em sistemas **Windows**.
- Suporta apenas o primeiro disco físico (geralmente `PhysicalDrive0`).
- É necessário acesso administrativo para operar.

## Contribuição

Contribuições são bem-vindas! Para colaborar:

1. **Relate bugs**: Abra issues detalhadas.
2. **Melhore o código**: Envie pull requests com suas alterações.
3. **Siga boas práticas**: Mantenha a qualidade e clareza no código.

## Licença

Este projeto é distribuído sob a Licença MIT.
