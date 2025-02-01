#ifndef OBJBOX_H
#define OBJBOX_H
/*
promote1:

# 我想改进对entity形成的Tree的路径的处理代码的改造：

## Entity路径和对象路径ObjPath：

Entity是对象的拥有着， 对象一定是在某个Entity路径下， 
ObjPath是另外一种索引对象的路径，在基于Entity路径的基础上，可以继续用对象成员来寻找对象。

如果对象成员类型是基本类型，那么对象路径就截止在这个基本类型。基本类型有int/uint64_t/float/bool....
如果成员对象类型是ObjPtr<T>, 那就表示另一个对象的位置，ObjPtr<T>含有Entity路径的信息来指向真实的对象。 
成员类型如果是包含ObjPtr<T>的容器， 那么通过容器的下标先索引到含有ObjPtr<T>的元素

Entity路径由Entity.h文件实现



## Entity和ObjPath对象路径树
Tree由基本的Entity路径加上ObjPath路径的扩展组成.

Entity或某个对象代表Tree的一个节点，每个节点的名字以A-Za-z0-9组成的字符串组成，Tree有一个根节点，每个节点都有个一个从根节点开始的绝对路径， 绝对路径的字符串表示从根节点开始节点名以分隔符"/"组成， 比如下面的Entity路径:
/foo/bar/car代表根节点开始， 根节点下的foo子节点， foo子节点下的bar子节点， bar下的子节点car
根节点名字在路径里不打印， 所有决定路径开头都是/符号
除了绝对路径还有相对路径， 相对路径是以非根节点开始的路径， 所以开发一定是非/符号。
有两个特殊的节点字符串：..代表父节点, .代表当前节点

Entity和ObjPath树由Tree.h文件实现

### Entity节点下的对象
Entity可以用有对象，但是同类型的对象只能有一个。
每个Entity节点拥有以类型为key， 值为对象的std::unordered_map的成员，通过这个成员来拥有对象。

### Storage

Entity节点下的对象都在统一的对象存储池里:
template<class T, class containter_type>
Storage;

而对象存储池的对象可以用std::shared_<T> 对象指针， 或者ObjPtr<T>封装类来指向。
ObjPtr<T>可以获得对应的Entity的信息,（通过EntityPool的hash)

由Storage.h实现


## ObjPath路径
每个Entity节点拥有的对象如果有子对象作为其成员， 那么子对象的成员名可以构成子对象ObjPath路径, ObjPath路径中的ObjPtr<T>类型的成员可以看成Entity路径的别名

ObjPath路径是加入对象类型， 和指向任意其他Entity构成的路径 
ObjPath也可以表示Entity路径。
- 比如有类型TypeA, TypeB
/foo:TypeA代表/foo Entity节点下的以类型TypeA对象构成路径， 假设对应的对象是objAf
/foo:TypeB代表/foo Entity节点下的以类型TypeB对象构成的路径， 假设对应的对象是objBf

/foo/bar:TypeA代表/foo/bar节点下的以类型TypeA对象构成的路径, 假设对应的对象是objAb
/foo/bar:TypeB代表/foo/bar节点下的以类型TypeB对象构成的路径, 假设对应的对象是objBb

假如
- objAf有成员peer_, 其类型是 TypeA的对象objAb
- objAb有成员peer_, 其类型是 TypeA的对象objAf

那么
- objAb的在objAf下的ObjPath路径: /foo:TypeA/peer_:TypeA, 这个路径代表对象ObjPtr<TypeA>，指向的对象是/foo/bar:TypeA的对象objAb， 也可以把/foo:TypeA/peer_看成/foo/bar的别名

- objAf的在objAb下的ObjPath路径: /foo/bar:TypeA/peer_:TypeA, 这个路径代表对象ObjPtr<TypeA>，指向的对象是/foo:TypeA的对象objAf， 也可以把/foo/bar:TypeA/peer_看成/foo的别名

## ObjPathPool
ObjPathPool通过hash来存储所有的ObjPath路径，

## ObjPath统一对Entity路径和对象路径的处理
从Tree的根路径开始，默认就是Entity路径，直到遇到foo:Type用:符号隔开的节点类型，再继续往下就是ObjPath路径. 为了处理方法， 统一用 ObjPath代表路径

### Entity路径是Object路径的子集
Entity是没有类型的路径，节点之间用/拼接
Object路径会在节点后出现类型，如果没有类型，也用:void来统一标识，比如
对象路径/foo/bar:TypeA/car:int, 可以统一的形式表示/foo:void/bar:TypeA/car:int
代表根节点下的foo子节点，foo子节点下的bar 子节点， bar子节点内的TypeA类型对象， 该对象下的int类型的car成员

### PathUtils添加统一的Object路径的处理

## ObjPtr<T>添加传统路径路径

ObjPtr<Ptr> 添加一个的std::string类型的员 object_path_, 在ObjPtr<T>相关函数使用ptr_之前判断是否为nullptr, 如果为nullptr, 那么通过object_path_为Object路径来填充ptr_

### ObjPtr<T> 对象初始化添加初始化object_path_的构造函数

## IniLoader对对象路径的初始化

有对象的 Object路径的leafname可以代表不同类型, 比如下面始不同leafname可能的内容
- /foo:TypeA/intval = int类型的数据
- /foo:TypeA/strval = std::string类型的数据
- /foo:TypeA/floatval = float类型的数据
- /foo:TypeA/boolval = bool类型的数据
- /foo:TypeA/doubleval = double类型的数据
- /foo:TypeA/uint64val = uint64_t类型的数据
- /foo:TypeA/obj_ptr_member = Object路径，可能是相对或绝对路径, 最后会生成ObjPtr<T>类型,指向路径所表示的对象
- /foo:TypeA/obj_ptr_member = json字符串，会在/foo Entity路径下创建对，然后在/foo:TypeA/obj_ptr_member生成ObjPtr<T>的对象, 该对象的Object路径指向创建的对象

## 设计要点
真实的对象一定是在某个Entity路径下吗， 而对象路径是通过ObjPtr<T>路表示一个对象的位置，ObjPtr<T>是通过Entity路径来指向真实的对象。 




*/
#endif
