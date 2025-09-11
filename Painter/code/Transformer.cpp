#include <drx/Painter/Painter.h>

namespace drx {

void LinearPathConsumer::End()
{
}

void LinearPathFilter::End()
{
	PutEnd();
}

void Transformer::Move(const Pointf& p)
{
	PutMove(xform.Transform(p));
}

void Transformer::Line(const Pointf& p)
{
	PutLine(xform.Transform(p));
}

}
