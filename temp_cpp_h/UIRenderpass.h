#pragma once

class VertexLayout
{
	public:
		enum class ElementType
		{
			Position2D,
			Position3D,
			Texture2D,
			Normal,
			Float3Color,
			Float4Color,
			BGRAColor
		}
		class Element
		{
			public:
				ElementType type;
				size_t offset;
			offset()
			offsetafter()
			size()
		}
		template <ElementType Type>
		//获得某个类型Type在 我们通过vertexlayout制造出来的Vertex 中的offset和类型信息。或者直接说获得某个elements中的元素得了
		const Element& Resolve() const
		{
			for e : elements	
				if  e.type == Type
					return e
			//如果没找到所要的属性，就直接报错
		}
		const Element& ResolveByIndex(size_t i) const{
			return elements[i];
		}

		AppendToElements()
		Size()
		std::vector<Element> elements;
}







class Vertex   //作用是对Vertex buffer形成一种view
{
	template<VertexLayout::ElementType Type> //值特化
	Attr() //获得当前顶点中某个属性中的引用
	{
		element = layout.Resolve<Type>();
		auto pAttribute = pData + element.GetOffset()
		if constexpr(Type == VertexLayout::Position2D ){
			return *reinterpret_cast<glm::vec2>(pAttribute);
		}
		..................
	}
	SetAttriByIndex()   //和上面那个函数差不多，只不过这个使用index来获取当前顶点中的某个属性的引用


	char*  pData;
	const VertexLayout& layout;

}




class VertexBuffer //一堆bytes
{
	VertexBuffer(VertexLayout){


	}

	Size(){
		buffer.size() / layout.size()   //获得一共有多少个我们通过vertexlayout 造出来的顶点
	}

	template<typename ...Params>
	void EmplaceBack(Params&&... params)
	{
		buffer.resize(buffer.size + layout.Size)   //根据VertexLayout增加一个顶点；
		Back().SetAttributeByIndex(0u,std::forward<Params>( params) ...)
	}

	Vertex Back()
	{
		return Vertex(buffer.data()+buffer.size() - layout.Size(),layout)   //获得当前最后一个Vertex的“View”
	}

	Vertex Front()
	{
	}







	std::vector<char> buffer;
	VertexLayout layout;


}