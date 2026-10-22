#include <iostream>                                   // الإدخال والإخراج (cout, cerr)
#include <fstream>                                    // قراءة الملفات (ifstream)
#include <string>                                     // استخدام النصوص string
using namespace std;                                  // لتجنب كتابة std:: في كل مرة

const int MAX_CITIES = 50;                            // أكبر عدد مدن مسموح به
const int MAX_ROADS = 1000;                           // أكبر عدد طرق مسموح به في الملف
const int MAX_STATES = 2 * MAX_ROADS + MAX_CITIES;    // سعة مصفوفات الحالات (frontier/visited)

// A search node: the city, the cost g(n) of reaching it, and the index of its parent
// in the visited/explored array (-1 for the origin). Parent links rebuild the route.
class State {
private:                                              // البيانات خاصة (Encapsulation)
    int city;                                         // رقم المدينة في قائمة المدن
    int cost;                                         // g(n): المسافة بالكيلومتر من نقطة البداية
    int parent;                                       // رقم الحالة الأب في المصفوفة (-1 للجذر)

public:                                               // الدوال العامة للتعامل مع الحالة
    State() : city(-1), cost(0), parent(-1) {}        // مُنشئ افتراضي (ضروري لتعريف المصفوفات)
    State(int cityIndex, int totalCost, int parentIndex)   // مُنشئ يعطي القيم عند إنشاء الكائن
        : city(cityIndex), cost(totalCost), parent(parentIndex) {}   // تخزين القيم في الحقول
    int getCity() const { return city; }              // إرجاع رقم المدينة
    int getCost() const { return cost; }              // إرجاع التكلفة g(n)
    int getParent() const { return parent; }          // إرجاع رقم الحالة الأب
    void setParent(int parentIndex) { parent = parentIndex; }   // تغيير رقم الأب
};                                                    // نهاية كلاس State
class RoadMap {
private:                                              // البيانات الداخلية مخفية عن بقية البرنامج
    string cityNames[MAX_CITIES];                     // أسماء المدن (مرتبة أبجدياً بعد القراءة)
    int roads[MAX_CITIES][MAX_CITIES];                // roads[a][b] = طول الطريق بالكم، 0 = لا يوجد طريق
    int cityCount;                                    // عدد المدن المختلفة
    int goalCity;                                     // رقم مدينة الوجهة (-1 إن لم توجد)
    bool loaded;                                      // هل تمت قراءة الملف بنجاح؟

    bool addCity(string name) {                       // إضافة مدينة جديدة إلى القائمة
        if (getCityIndex(name) != -1) return true;    // المدينة موجودة مسبقاً فلا نكررها
        if (cityCount >= MAX_CITIES) return false;   // لا يوجد مكان لمدينة جديدة
        cityNames[cityCount] = name;                  // نخزن الاسم في أول مكان فارغ
        cityCount++;                                  // نزيد عدد المدن
        return true;                                  // تمت الإضافة
    }                                                 // نهاية addCity

    bool readFile(string fileName) {                  // قراءة ملف الطرق وبناء الخريطة
        ifstream file(fileName.c_str());              // فتح الملف للقراءة
        if (!file) {                                  // إذا فشل فتح الملف
            cout << "Cannot open file " << fileName << "\n";   // رسالة خطأ للمستخدم
            return false;                             // فشلت القراءة
        }

        string fromCity[MAX_ROADS], toCity[MAX_ROADS];   // مدينة البداية والنهاية لكل طريق
        int distance[MAX_ROADS];                      // طول كل طريق بالكيلومتر
        int roadCount = 0;                            // عدد الطرق المقروءة

        string word;                                  // الكلمة المقروءة حالياً
        while (file >> word) {                        // نقرأ كلمة كلمة حتى نهاية الملف
            if (word == "END") break;                 // وصلنا إلى "END OF INPUT" فنتوقف

            if (roadCount >= MAX_ROADS) {             // الملف فيه طرق أكثر من المسموح
                cout << "Too many roads in the input file (maximum " << MAX_ROADS << ")\n";   // رسالة خطأ
                return false;                         // فشلت القراءة
            }
            fromCity[roadCount] = word;               // الكلمة الأولى هي مدينة البداية
            if (!(file >> toCity[roadCount] >> distance[roadCount])) {   // قراءة مدينة النهاية والمسافة
                cout << "Invalid line in the input file\n";   // السطر ناقص أو تالف
                return false;                         // فشلت القراءة
            }
            roadCount++;                              // طريق جديد تمت قراءته
        }

        for (int i = 0; i < roadCount; i++) {         // نمر على كل الطرق لاستخراج المدن
            if (!addCity(fromCity[i]) || !addCity(toCity[i])) {   // إضافة المدينتين إن كانتا جديدتين
                cout << "Too many cities in the input file (maximum " << MAX_CITIES << ")\n";   // تجاوز الحد
                return false;                         // فشلت القراءة
            }
        }

        for (int i = 0; i < cityCount - 1; i++) {     // ترتيب فقاعي للمدن أبجدياً
            for (int j = 0; j < cityCount - 1 - i; j++) {   // مقارنة كل مدينة مع التي تليها
                if (cityNames[j] > cityNames[j + 1]) {      // إذا كانتا بترتيب خاطئ
                    string temp = cityNames[j];             // نحفظ الأولى مؤقتاً
                    cityNames[j] = cityNames[j + 1];        // نضع الثانية مكان الأولى
                    cityNames[j + 1] = temp;                // نضع الأولى مكان الثانية
                }
            }
        }

        for (int i = 0; i < roadCount; i++) {         // تعبئة مصفوفة المسافات
            int a = getCityIndex(fromCity[i]);        // رقم مدينة البداية بعد الترتيب
            int b = getCityIndex(toCity[i]);          // رقم مدينة النهاية بعد الترتيب
            roads[a][b] = distance[i];                // الطريق من a إلى b
            roads[b][a] = distance[i];                // والطريق بالاتجاه المعاكس (طرق ثنائية الاتجاه)
        }

        return true;                                  // تمت القراءة بنجاح
    }                                                 // نهاية readFile

public:                                               // الدوال التي يستخدمها بقية البرنامج
    
    RoadMap(string fileName, string goalCityName) {   // المُنشئ: يستقبل ملف الطرق ومدينة الوجهة
        cityCount = 0;                                // لا توجد مدن في البداية
        goalCity = -1;                                // لا توجد وجهة بعد
        for (int i = 0; i < MAX_CITIES; i++) {        // تصفير مصفوفة الطرق
            for (int j = 0; j < MAX_CITIES; j++) {    // كل خانة في الصف
                roads[i][j] = 0;                      // 0 تعني لا يوجد طريق
            }
        }
        loaded = readFile(fileName);                  // قراءة الملف وحفظ النتيجة
        if (loaded) goalCity = getCityIndex(goalCityName);   // تحديد رقم مدينة الوجهة
    }                                                 // نهاية المُنشئ

    bool isLoaded() const { return loaded; }          // هل قُرئ الملف بنجاح؟
    int getCityCount() const { return cityCount; }    // عدد المدن
    int getGoalCity() const { return goalCity; }      // رقم مدينة الوجهة (-1 إن لم توجد)
    string getCityName(int index) const { return cityNames[index]; }   // اسم المدينة من رقمها
    int getRoadLength(int a, int b) const { return roads[a][b]; }      // طول الطريق بين مدينتين (0 = لا طريق)

    // Returns the index of a city, or -1 if the city is not in the map.
    int getCityIndex(string name) const {             // البحث عن رقم مدينة بالاسم
        for (int i = 0; i < cityCount; i++) {         // نمر على كل المدن
            if (cityNames[i] == name)                 // إذا تطابق الاسم
                return i;                             // نرجع رقمها
        }
        return -1;                                    // المدينة غير موجودة
    }                                                 // نهاية getCityIndex

    
    bool isGoal(State state) const {                  // اختبار الهدف
        return state.getCity() == goalCity;           // هل مدينة الحالة هي الوجهة؟
    }

    // Transition function: goes to the neighbour "nextCity" and adds that road's length to g(n).
    State move(State state, int nextCity) const {     // الانتقال من حالة إلى مدينة مجاورة
        int newCost = state.getCost() + roads[state.getCity()][nextCity];   // التكلفة الجديدة = القديمة + طول الطريق
        return State(nextCity, newCost, state.getParent());   // حالة جديدة في المدينة المجاورة
    }
};                                                    // نهاية كلاس RoadMap


class Algorithms {
private:                                              // الحقول الداخلية مخفية (Encapsulation)
    RoadMap &roadMap;                                 // الخريطة التي نبحث فيها (مرجع)
    int expandedCount;                                // عدد العقد الموسَّعة
    int generatedCount;                               // عدد العقد المولَّدة
    int maxFrontierSize;                              // أكبر عدد عقد محفوظة معاً: frontier في BFS/UCS،
                                                      // عمق الاستدعاء في DFS، طول المسار في IDT
    int currentDepth;                                 // العمق الحالي (يستخدمه DFS فقط)

    
    bool isSame(State a, State b) {                   // مقارنة حالتين
        return a.getCity() == b.getCity();            // نفس المدينة = نفس الحالة
    }

    // True if a state with the same city is already in the array (used for Graph-Search).
    bool exists(State states[], int size, State state) {   // هل الحالة موجودة في المصفوفة؟
        for (int i = 0; i < size; i++) {              // نمر على عناصر المصفوفة
            if (isSame(states[i], state))             // إذا وجدنا نفس المدينة
                return true;                          // موجودة
        }
        return false;                                 // غير موجودة
    }

    void printPath(State states[], int index, RoadMap &map) {   // طباعة المسار من البداية حتى الحالة index
        int parentIndex = states[index].getParent();  // رقم الحالة الأب
        if (parentIndex == -1) return;                // وصلنا إلى مدينة البداية فنتوقف

        printPath(states, parentIndex, map);          // نطبع أولاً المسار حتى الأب
        cout << map.getCityName(states[parentIndex].getCity()) << " to "   // اسم مدينة الأب
             << map.getCityName(states[index].getCity()) << ", "           // اسم المدينة الحالية
             << states[index].getCost() - states[parentIndex].getCost() << " km\n";   // طول هذا الطريق
    }

    void printSolution(State states[], int index, RoadMap &map) {   // طباعة الحل كاملاً
        cout << "distance: " << states[index].getCost() << " km\n"; // المسافة الكلية
        cout << "route:\n";                           // عنوان المسار
        printPath(states, index, map);                // طباعة طرق المسار واحداً واحداً
    }

    bool DFS_util(State current, State visited[], int &visitedCount) {   // الدالة العودية لـ DFS
        visited[visitedCount] = current;              // نسجل الحالة كمزارة
        int index = visitedCount;                     // نحفظ رقمها في المصفوفة
        visitedCount++;                               // نزيد عدد المزارة

        expandedCount++;                              // عقدة موسَّعة جديدة
        cerr << roadMap.getCityName(current.getCity()) << " ";   // طباعة ترتيب التوسيع

        currentDepth++;                               // زاد العمق بمقدار واحد
        if (currentDepth > maxFrontierSize) maxFrontierSize = currentDepth;   // تحديث أكبر عمق

        if (roadMap.isGoal(current)) {                // هل وصلنا للهدف؟
            cerr << "\n";                             // إنهاء سطر ترتيب التوسيع
            printSolution(visited, index, roadMap);   // طباعة الحل
            return true;                              // نجح البحث
        }

        for (int i = 0; i < roadMap.getCityCount(); i++) {   // نجرّب كل المدن المجاورة بالترتيب الأبجدي
            if (roadMap.getRoadLength(current.getCity(), i) > 0) {   // هل يوجد طريق إلى المدينة i؟
                State nextState = roadMap.move(current, i);          // الانتقال إلى المدينة i
                nextState.setParent(index);           // الحالة الحالية هي الأب
                if (!exists(visited, visitedCount, nextState)) {     // إذا لم نزرها من قبل
                    generatedCount++;                 // عقدة مولَّدة جديدة
                    if (DFS_util(nextState, visited, visitedCount))  // ننزل أعمق
                        return true;                  // وجدنا الحل فنوقف البحث
                }
            }
        }

        currentDepth--;                               // طريق مسدود: نرجع خطوة (backtrack)
        return false;                                 // لم نجد الحل من هذه العقدة
    }

    bool DLS(State current, State visited[], int &visitedCount, int limit) {   // بحث بعمق محدود
        if (exists(visited, visitedCount, current)) return false;   // المدينة على المسار الحالي (دورة)

        visited[visitedCount] = current;              // نضيف المدينة إلى المسار الحالي
        int index = visitedCount;                     // رقمها في المسار
        visitedCount++;                               // زاد طول المسار

        expandedCount++;                              // عقدة موسَّعة جديدة
        cerr << roadMap.getCityName(current.getCity()) << " ";   // طباعة ترتيب التوسيع

        if (visitedCount > maxFrontierSize) maxFrontierSize = visitedCount;   // تحديث أكبر مسار

        if (roadMap.isGoal(current)) {                // هل وصلنا للهدف؟
            cerr << "\n";                             // إنهاء سطر ترتيب التوسيع
            printSolution(visited, index, roadMap);   // طباعة الحل
            return true;                              // نجح البحث
        }

        if (limit <= 0) {                             // وصلنا إلى حد العمق (cutoff)
            visitedCount--;                           // نزيل المدينة من المسار
            return false;                             // لا ننزل أعمق
        }

        for (int i = 0; i < roadMap.getCityCount(); i++) {   // نجرّب كل المدن المجاورة بالترتيب الأبجدي
            if (roadMap.getRoadLength(current.getCity(), i) > 0) {   // هل يوجد طريق إلى المدينة i؟
                State nextState = roadMap.move(current, i);          // الانتقال إلى المدينة i
                nextState.setParent(index);           // الحالة الحالية هي الأب
                generatedCount++;                     // عقدة مولَّدة جديدة
                if (DLS(nextState, visited, visitedCount, limit - 1))   // بحث أعمق بحد أقل بواحد
                    return true;                      // وجدنا الحل فنوقف البحث
            }
        }

        visitedCount--;                               // backtrack: نزيل هذه المدينة من المسار
        return false;                                 // لم نجد الحل من هذه العقدة
    }

public:                                               // الدوال التي يستدعيها main
    Algorithms(RoadMap &map) : roadMap(map) {         // المُنشئ: يستقبل الخريطة ويحفظ مرجعاً لها
        expandedCount = 0;                            // العدادات تبدأ من صفر
        generatedCount = 0;                           // عدد العقد المولَّدة
        maxFrontierSize = 0;                          // أكبر عدد عقد في الذاكرة
        currentDepth = 0;                             // العمق الحالي
    }

    int getExpandedCount() const { return expandedCount; }       // عدد العقد الموسَّعة
    int getGeneratedCount() const { return generatedCount; }     // عدد العقد المولَّدة
    int getMaxFrontierSize() const { return maxFrontierSize; }   // أكبر عدد عقد في الذاكرة

    void printNoRoute() {                             // طباعة حالة عدم وجود طريق
        cout << "distance: infinity\n";               // المسافة لا نهائية
        cout << "Route: none\n";                      // لا يوجد مسار
    }

    void BFS(State start) {                           // البحث بالعرض أولاً
        State queue[MAX_STATES];                      // طابور FIFO للحدود (frontier)
        State visited[MAX_STATES];                    // الحالات الموسَّعة (لإعادة بناء المسار)

        int front = 0, rear = 0, visitedCount = 0;    // بداية الطابور ونهايته وعدد المزارة

        start.setParent(-1);                          // البداية ليس لها أب
        queue[rear++] = start;                        // نضع البداية في الطابور
        generatedCount++;                             // عقدة مولَّدة

        cerr << "Expansion order: ";                  // عنوان ترتيب التوسيع

        while (front < rear) {                        // نستمر ما دام الطابور غير فارغ

            if (rear - front > maxFrontierSize) maxFrontierSize = rear - front;   // تحديث أكبر حجم للطابور

            State current = queue[front++];           // نأخذ أقدم عقدة (الأقل عمقاً)

            if (exists(visited, visitedCount, current))   // إذا وُسِّعت مدينتها من قبل
                continue;                             // نتجاهلها

            visited[visitedCount] = current;          // نسجلها كموسَّعة
            int index = visitedCount;                 // رقمها في المصفوفة
            visitedCount++;                           // زاد عدد الموسَّعة

            expandedCount++;                          // عقدة موسَّعة جديدة
            cerr << roadMap.getCityName(current.getCity()) << " ";   // طباعة ترتيب التوسيع

            if (roadMap.isGoal(current)) {            // اختبار الهدف عند التوسيع
                cerr << "\n";                         // إنهاء سطر ترتيب التوسيع
                printSolution(visited, index, roadMap);   // طباعة الحل
                return;                               // انتهى البحث
            }

            for (int i = 0; i < roadMap.getCityCount(); i++) {   // المدن المجاورة بالترتيب الأبجدي
                if (roadMap.getRoadLength(current.getCity(), i) > 0) {   // هل يوجد طريق؟
                    State nextState = roadMap.move(current, i);          // الانتقال إلى المدينة i
                    if (!exists(visited, visitedCount, nextState) &&     // لم تُوسَّع بعد
                        !exists(queue, rear, nextState)) {               // وليست في الطابور
                        nextState.setParent(index);   // الحالة الحالية هي الأب
                        queue[rear++] = nextState;    // نضيفها إلى آخر الطابور
                        generatedCount++;             // عقدة مولَّدة جديدة
                    }
                }
            }
        }

        cerr << "\n";                                 // إنهاء سطر ترتيب التوسيع
        printNoRoute();                               // الطابور فرغ: لا يوجد طريق
    }

    void DFS(State start) {                           // البحث بالعمق أولاً
        State visited[MAX_STATES];                    // المدن المزارة
        int visitedCount = 0;                         // عددها

        start.setParent(-1);                          // البداية ليس لها أب
        generatedCount++;                             // عقدة مولَّدة (البداية)

        cerr << "Expansion order: ";                  // عنوان ترتيب التوسيع
        if (!DFS_util(start, visited, visitedCount)) {   // نبدأ البحث العودي
            cerr << "\n";                             // إنهاء سطر ترتيب التوسيع
            printNoRoute();                           // لم نجد طريقاً
        }
    }

    void IDT(State start) {                           // البحث بالتعميق التكراري
        for (int depth = 0; depth < roadMap.getCityCount(); depth++) {   // حد العمق: 0 ثم 1 ثم 2 ...

            State visited[MAX_STATES];                // المسار الحالي
            int visitedCount = 0;                     // طوله

            start.setParent(-1);                      // البداية ليس لها أب
            generatedCount++;                         // عقدة مولَّدة (البداية في كل تكرار)

            cerr << "Depth limit " << depth << ": ";  // طباعة حد العمق الحالي

            if (DLS(start, visited, visitedCount, depth))   // بحث بعمق محدود
                return;                               // وجدنا الحل

            cerr << "\n";                             // إنهاء سطر هذا التكرار
        }

        printNoRoute();                               // لا يوجد حل بأي عمق
    }

    void UCS(State start) {                           // البحث بالتكلفة الموحدة
        State frontier[MAX_STATES];                   // قائمة الأولوية (مرتبة حسب g(n))
        State explored[MAX_STATES];                   // الحالات الموسَّعة (لإعادة بناء المسار)

        int frontierCount = 0, exploredCount = 0;     // عدد عناصر كل مصفوفة

        start.setParent(-1);                          // البداية ليس لها أب
        frontier[frontierCount++] = start;            // نضع البداية في القائمة
        generatedCount++;                             // عقدة مولَّدة

        cerr << "Expansion order: ";                  // عنوان ترتيب التوسيع

        while (frontierCount > 0) {                   // نستمر ما دامت القائمة غير فارغة

            if (frontierCount > maxFrontierSize) maxFrontierSize = frontierCount;   // تحديث أكبر حجم

            int best = 0;                             // رقم أفضل عقدة (أقل تكلفة)
            for (int i = 1; i < frontierCount; i++) { // نبحث عن أقل تكلفة
                if (frontier[i].getCost() < frontier[best].getCost() ||      // تكلفة أقل
                   (frontier[i].getCost() == frontier[best].getCost() &&     // أو تكلفة متساوية
                    frontier[i].getCity() < frontier[best].getCity()))       // ومدينة أسبق أبجدياً
                    best = i;                         // هذه أفضل عقدة حتى الآن
            }

            State current = frontier[best];           // نأخذ العقدة الأفضل

            for (int i = best; i < frontierCount - 1; i++) {   // نحذفها من القائمة بإزاحة ما بعدها
                frontier[i] = frontier[i + 1];        // إزاحة عنصر واحد
            }
            frontierCount--;                          // نقص عدد عناصر القائمة

            if (exists(explored, exploredCount, current))   // مدينتها وُسِّعت بتكلفة أقل من قبل
                continue;                             // نتجاهل هذه النسخة

            explored[exploredCount] = current;        // نسجلها كموسَّعة
            int index = exploredCount;                // رقمها في المصفوفة
            exploredCount++;                          // زاد عدد الموسَّعة

            expandedCount++;                          // عقدة موسَّعة جديدة
            cerr << roadMap.getCityName(current.getCity()) << " ";   // طباعة ترتيب التوسيع

            if (roadMap.isGoal(current)) {            // اختبار الهدف عند التوسيع (يضمن الحل الأمثل)
                cerr << "\n";                         // إنهاء سطر ترتيب التوسيع
                printSolution(explored, index, roadMap);   // طباعة الحل
                return;                               // انتهى البحث
            }

            for (int i = 0; i < roadMap.getCityCount(); i++) {   // المدن المجاورة بالترتيب الأبجدي
                if (roadMap.getRoadLength(current.getCity(), i) > 0) {   // هل يوجد طريق؟
                    State nextState = roadMap.move(current, i);          // الانتقال وزيادة التكلفة
                    if (!exists(explored, exploredCount, nextState)) {   // لم تُوسَّع بعد
                        nextState.setParent(index);   // الحالة الحالية هي الأب
                        frontier[frontierCount++] = nextState;   // نضيفها إلى القائمة
                        generatedCount++;             // عقدة مولَّدة جديدة
                    }
                }
            }
        }

        cerr << "\n";                                 // إنهاء سطر ترتيب التوسيع
        printNoRoute();                               // القائمة فرغت: لا يوجد طريق
    }
};                                                    // نهاية كلاس Algorithms

int main(int argc, char* argv[]) {                    // نقطة بداية البرنامج

    string algorithm, fileName, originCity, destinationCity;   // قيم سطر الأوامر

    if (argc == 5) {                                  // الحالة العادية: 4 معاملات
        algorithm = argv[1];                          // اسم الخوارزمية
        fileName = argv[2];                           // اسم ملف الطرق
        originCity = argv[3];                         // مدينة البداية
        destinationCity = argv[4];                    // مدينة الوجهة
    }
    else if (argc == 4) {                             // تم حذف اسم الخوارزمية: نستخدم UCS افتراضياً
        algorithm = "UCS";                            // الخوارزمية الافتراضية
        fileName = argv[1];                           // اسم ملف الطرق
        originCity = argv[2];                         // مدينة البداية
        destinationCity = argv[3];                    // مدينة الوجهة
    }
    else {                                            // عدد معاملات خاطئ
        cout << "Usage: find_route algorithm input_filename origin_city destination_city\n";   // طريقة الاستخدام
        cout << "algorithm = BFS | DFS | IDT | UCS\n";   // أسماء الخوارزميات المسموحة
        return 1;                                     // خروج مع رمز خطأ
    }

    RoadMap roadMap(fileName, destinationCity);       // إنشاء الخريطة: ملف الطرق + مدينة الوجهة
    if (!roadMap.isLoaded()) {                        // فشلت قراءة الملف
        return 1;                                     // خروج مع رمز خطأ (الرسالة طُبعت)
    }

    Algorithms algorithms(roadMap);                   // إنشاء كائن الخوارزميات على هذه الخريطة

    int startCity = roadMap.getCityIndex(originCity); // رقم مدينة البداية

    if (startCity == -1 || roadMap.getGoalCity() == -1) {   // مدينة البداية أو الوجهة غير موجودة
        algorithms.printNoRoute();                    // لا يوجد طريق
        return 0;                                     // انتهى البرنامج
    }

    State start(startCity, 0, -1);                    // حالة البداية: تكلفة 0 وبلا أب

    if (algorithm == "BFS")                           // اختيار الخوارزمية حسب الاسم
        algorithms.BFS(start);                        // البحث بالعرض
    else if (algorithm == "DFS")                      // البحث بالعمق
        algorithms.DFS(start);
    else if (algorithm == "IDT" || algorithm == "IDS")   // التعميق التكراري (IDS اسم بديل)
        algorithms.IDT(start);
    else if (algorithm == "UCS")                      // البحث بالتكلفة الموحدة
        algorithms.UCS(start);
    else {                                            // اسم خوارزمية غير معروف
        cout << "Unknown algorithm. Use BFS, DFS, IDT or UCS\n";   // رسالة خطأ
        return 1;                                     // خروج مع رمز خطأ
    }

    cerr << "Nodes expanded  : " << algorithms.getExpandedCount() << "\n";    // عدد العقد الموسَّعة
    cerr << "Nodes generated : " << algorithms.getGeneratedCount() << "\n";   // عدد العقد المولَّدة
    cerr << "Max nodes in memory : " << algorithms.getMaxFrontierSize() << "\n";   // أكبر عدد في الذاكرة

    return 0;                                         // انتهاء ناجح
}                                                     // نهاية main
