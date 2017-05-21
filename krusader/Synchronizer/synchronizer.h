/*****************************************************************************
 * Copyright (C) 2003 Csaba Karai <krusader@users.sourceforge.net>           *
 * Copyright (C) 2004-2018 Krusader Krew [https://krusader.org]              *
 *                                                                           *
 * This file is part of Krusader [https://krusader.org].                     *
 *                                                                           *
 * Krusader is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation, either version 2 of the License, or         *
 * (at your option) any later version.                                       *
 *                                                                           *
 * Krusader is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License for more details.                              *
 *                                                                           *
 * You should have received a copy of the GNU General Public License         *
 * along with Krusader.  If not, see [http://www.gnu.org/licenses/].         *
 *****************************************************************************/

#ifndef SYNCHRONIZER_H
#define SYNCHRONIZER_H

// QtCore
#include <QList>
#include <QMap>
#include <QObject>
// QtGui
#include <QColor>
// QtWidgets
#include <QDialog>

#include <KIO/Job>

#include "synchronizerfileitem.h"
#include "../FileSystem/krquery.h"

class CompareTask;
class FileItem;
class FileSearcher;
class SynchronizerTask;

class Synchronizer : public QObject
{
    Q_OBJECT

private:
    int displayUpdateCount; // the display is refreshed after every x-th change

public:
    Synchronizer();
    ~Synchronizer();

    int compare(const QUrl &left, const QUrl &right, const KRQuery &query, bool subDirs, bool symLinks,
                bool igDate, bool asymm, bool cmpByCnt, bool igCase, bool autoSc,
                QStringList &selFiles, int equThres, int timeOffs, int parThreads);
    void stop() { stopped = true; }
    void setMarkFlags(bool left, bool equal, bool differs, bool right, bool dup, bool sing,
                      bool del);
    int refresh(bool nostatus = false);
    bool totalSizes(int *, KIO::filesize_t *, int *, KIO::filesize_t *, int *, KIO::filesize_t *);

    void synchronize(QWidget *syncDialog, bool leftCopyEnabled, bool rightCopyEnabled,
                     bool deleteEnabled, bool overWrite, int parThreads);
    /**
     * Execute sync with KGet.
     *
     * This is only supported for downloading files from one remote side other local side.
     * Local destination files (if exist) are removed before downloading!
     */
    void synchronizeWithKGet();

    void setScrolling(bool scroll);
    void pause();
    void resume();
    void swapSides();
    void reset();
    void clearLists();

    void exclude(SynchronizerFileItem *);
    void restore(SynchronizerFileItem *);
    void reverseDirection(SynchronizerFileItem *);
    void copyToLeft(SynchronizerFileItem *);
    void copyToRight(SynchronizerFileItem *);
    void deleteLeft(SynchronizerFileItem *);

    const QUrl leftBaseDirectory() { return leftBaseDir; }
    const QUrl rightBaseDirectory() { return rightBaseDir; }

    static QUrl pathAppend(const QUrl &url, const QString &fileName);
    static QUrl pathAppend(const QUrl &url, const QString &dirName, const QString &fileName);

    static QString getTaskTypeName(TaskType taskType);


    SynchronizerFileItem *getItemAt(unsigned ndx);

    void setParentWidget(QWidget *widget) { parentWidget = widget; }
    void compareContentResult(SynchronizerFileItem *item, bool result);

signals:
    void comparedFileData(SynchronizerFileItem *);
    void markChanged(SynchronizerFileItem *, bool);
    void synchronizationFinished();
    void processedSizes(int, KIO::filesize_t, int, KIO::filesize_t, int, KIO::filesize_t);
    void pauseAccepted();
    void statusInfo(QString);

public slots:
    void slotTaskFinished(KJob *);
    void slotProcessedSize(KJob *, qulonglong);

private:
    bool isDir(const FileItem *file);

    /** Evaluate the result directories of a finished(ready) compare task with both sides.*/
    void compareDirectory(SynchronizerFileItem *parent, const QUrl &leftUrl, const QUrl &rightUrl,
                          const QList<FileItem *> &leftFiles, const QList<FileItem *> &rightFiles,
                          const QString &leftDir, const QString &rightDir);

    SynchronizerFileItem *addItem(FileItem *leftFile, FileItem *rightFile,
                                  SynchronizerFileItem *parent, const QString &leftDir,
                                  const QString &rightDir, TaskType tsk, bool isTemp);

    SynchronizerFileItem *addLeftOnlyItem(FileItem *leftFile, SynchronizerFileItem *parent,
                                          const QString &dir, bool isTemp = false);
    SynchronizerFileItem *addRightOnlyItem(FileItem *rightFile, SynchronizerFileItem *parent,
                                           const QString &dir, bool isTemp = false);
    SynchronizerFileItem *addDuplicateItem(FileItem *leftFile, FileItem *rightFile,
                                           SynchronizerFileItem *parent,
                                           const QString &leftDir, const QString &rightDir,
                                           bool isTemp = false);
    void addSingleDirectory(SynchronizerFileItem *parent, const QUrl &url, const QList<FileItem *> &files,
                            const QString &dirName, bool isLeft);

    bool isMarked(TaskType task, bool dupl);
    bool markParentDirectories(SynchronizerFileItem *);
    void synchronizeLoop();
    SynchronizerFileItem *getNextTask();
    void executeTask(SynchronizerFileItem *task);
    KJob *createJob(SynchronizerFileItem *task, const QUrl &src, const QUrl &url, const QString &link);
    void setPermanent(SynchronizerFileItem *);
    void operate(SynchronizerFileItem *item, void (*)(SynchronizerFileItem *));
    void compareLoop();

    static void excludeOperation(SynchronizerFileItem *item);
    static void restoreOperation(SynchronizerFileItem *item);
    static void reverseDirectionOperation(SynchronizerFileItem *item);
    static void copyToLeftOperation(SynchronizerFileItem *item);
    static void copyToRightOperation(SynchronizerFileItem *item);
    static void deleteLeftOperation(SynchronizerFileItem *item);

    static const QHash<QString, FileItem *> createFileSet(const QList<FileItem *> &files, bool ignoreCase);

protected:
    bool recurseSubDirs;                         // walk through subdirectories also
    bool followSymLinks;                         // follow the symbolic links
    bool ignoreDate;                             // don't use date info at comparing
    bool asymmetric;                             // asymmetric directory update
    bool cmpByContent;                           // compare the files by content
    bool ignoreCase;                             // case insensitive synchronization for Windows fs
    bool autoScroll;                             // automatic update of the directory
    QList<SynchronizerFileItem *> resultList;    // the found files
    QList<SynchronizerFileItem *> temporaryList; // temporary files
    QUrl leftBaseDir;                            // the left-side base directory
    QUrl rightBaseDir;                           // the right-side base directory
    QStringList excludedPaths;                   // list of the excluded paths
    KRQuery query;                              // the filter used for the query
    bool stopped;                                // 'Stop' button was pressed

    int equalsThreshold; // threshold to treat files equal
    int timeOffset;      // time offset between the left and right sides

    bool markEquals;      // show the equal files
    bool markDiffers;     // show the different files
    bool markCopyToLeft;  // show the files to copy from right to left
    bool markCopyToRight; // show the files to copy from left to right
    bool markDeletable;   // show the files to be deleted
    bool markDuplicates;  // show the duplicated items
    bool markSingles;     // show the single items

    bool leftCopyEnabled;  // copy to left is enabled at synchronize
    bool rightCopyEnabled; // copy to right is enabled at synchronize
    bool deleteEnabled;    // delete is enabled at synchronize
    bool overWrite;        // overwrite or query each modification
    bool autoSkip;         // automatic skipping
    bool paused;           // pause flag
    bool disableNewTasks;  // at mkdir the further task creation is disabled

    int leftCopyNr;                // the file number copied to left
    int rightCopyNr;               // the file number copied to right
    int deleteNr;                  // the number of the deleted files
    int parallelThreads;           // the number of the parallel procesing threads
    KIO::filesize_t leftCopySize;  // the total size copied to left
    KIO::filesize_t rightCopySize; // the total size copied to right
    KIO::filesize_t deleteSize;    // the size of the deleted files

    int comparedDirs; // the number of the compared directories
    int fileCount;    // the number of counted files

private:
    QList<SynchronizerTask *> stack;             // stack for comparing
    QMap<KJob *, SynchronizerFileItem *> jobMap; // job maps
    QMap<KJob *, KIO::filesize_t> receivedMap;   // the received file size
    SynchronizerFileItem *lastTask;              // reference to the last stack
    int inTaskFinished;                          // counter of quasy 'threads' in slotTaskFinished

    QStringList selectedFiles;                          // the selected files to compare
    QWidget *parentWidget;                              // the parent widget
    QWidget *syncDlgWidget;                             // the synchronizer dialog widget
    QListIterator<SynchronizerFileItem *> resultListIt; // iterator for result list
};

class QProgressBar;

class KgetProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KgetProgressDialog(QWidget *parent = 0, const QString &caption = QString(),
                                const QString &text = QString(), bool modal = false);

    QProgressBar *progressBar() { return mProgressBar; }

public slots:
    void slotPause();
    void slotCancel();

    bool wasCancelled() { return mCancelled; }
    bool isPaused() { return mPaused; }

private:
    QProgressBar *mProgressBar;
    QPushButton *mPauseButton;
    bool mCancelled;
    bool mPaused;
};

#endif /* __SYNCHRONIZER_H__ */
