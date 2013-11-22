
#ifndef UNDOREDO_H
#define UNDOREDO_H

#include <stack>
#include <string>
#include <vector>

#include <QObject>

#include "primitives.h"
#include "canetemplate.h"
#include "casing.h"
#include "subcanetemplate.h"

class QVBoxLayout;

class MainWindow;
class GlassColor;
class Cane;
class Piece;

using std::string;
using std::stack;
using std::vector;

class UndoRedo : public QObject
{
	Q_OBJECT

	public:
		enum EventType
		{
			ADD,
			DELETE,
			MODIFY,
			MOVE,
			NO_UNDO
		};

		enum ObjectType
		{
			GLASSCOLOR,
			CANE,
			PIECE
		};

		struct Event
		{
			enum EventType type;	
			enum ObjectType objectType;	

			unsigned int index;
			int direction;

			GlassColor* glassColor;
			GlassColor* preGlassColorState;
			GlassColor* postGlassColorState;

			Cane* cane;
			Cane* preCaneState;
			Cane* postCaneState;
		};

		UndoRedo(MainWindow* mw);
		void addedGlassColor(GlassColor* gc, unsigned int index);
		void deletedGlassColor(GlassColor* gc, unsigned int index);
		void modifiedGlassColor(GlassColor* gc);
		void movedGlassColor(unsigned int index, int direction);
		void addedCane(Cane* c, unsigned int index);
		void deletedCane(Cane* c, unsigned int index);
		void modifiedCane(Cane* c);
		void movedCane(unsigned int index, int direction);		

		/*
		void newPiece(Piece* p);
		void deletePiece(Piece* p);
		void modifiedPiece(Piece* p);
		void movedPiece(unsigned int index, int direction);
		*/

		void noPriorUndo();
		void clearHistory();

	public slots:
		void undo();
		void redo();
	
	private:
		stack<struct Event> undoStack;
		stack<struct Event> redoStack;

		MainWindow* mainWindow;	
		
		static void initEvent(struct Event& event);
		void clearRedoStack();
		void clearUndoStack();
		void undoGlassColorEvent(struct Event& event);
		void undoCaneEvent(struct Event& event);
		void redoGlassColorEvent(struct Event& event);
		void redoCaneEvent(struct Event& event);
		bool canUndo();
		bool canRedo();
};

#endif

