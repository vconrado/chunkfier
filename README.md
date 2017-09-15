# Chunkfier

Esse programa converte um cubo de dados (ex: x, y e z) armazenado sequêncialmente (x, y e z) em um arquivo estruturado em *chunks*. Para a criação dos *chunks* o cubo é percorrido em *z*, *y* e *x*. 
Esse programa foi desenvolvido com o objetivo de auxiliar na preparação de dados para serem carregados no [SciDB](http://forum.paradigm4.com/).

## Instalação
```bash
git clone https://github.com/vconrado/chunkfier.git
cd chunkfier/build
./configure
make
make install

```

## Usando o Chunkfier

Para usar o **chunkfier**, chame o programa passando como argumentos os tamanhos de cada dimensão (x, y e z), cada um seguido pelo tamanho da *chunk*. 

```bash
Usage: chunkfier X-size X-chunk-size Y-size Y-chunk-size Z-size Z-chunk-size data-size input_file output_file
```

Por exemplo, para um cubo de dados de dimensões (X,Y,Z) = (1200,1400,500) para criar um arquivo com *chunks* com as dimensões (x,y,z)=(100, 200, 250), utilize:
```bash
chunkfier 1200 100 1400 200 500 250 input_file output_file
```


